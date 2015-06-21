var GeometryMode = Object.freeze({
  PATH: 'PATH',
  SURFACE: 'SURFACE'
});

THREE.Object3D.prototype.clear = function() {
  var children = this.children;
  for (var i = children.length - 1; i >= 0; --i) {
    var child = children[i];
    child.clear();
    this.remove(child);
  }
}

var scene, renderer, camera, controls;
var geoscene, pointerScene;
var meshes = [];
var nSecondsTillPreview = 1.0;
var showWireframe = false;
var arrowShafts = [];
var showHeadings = true;
var modelColor = 'FF0000';
var workspace = null;
var fontSize = 14;
var gridSpacing = 1.0;
var gridExtent = 10.0;

var isShowWireframeChanged = false;
var isAxisChanged = [false, false, false];
var isPlaneChanged = [false, false, false];
var isNSecondsTillPreviewChanged = false;
var isShowHeadingsChanged = false;
var isModelColorChanged = false;
var isFontSizeChanged = false;
var isGridSpacingChanged = false;
var isGridExtentChanged = false;

function saveInCookies() {
  $.cookie('last', getSource());
  if (workspace) {
    var xml = Blockly.Xml.workspaceToDom(workspace);
    var xmlText = Blockly.Xml.domToText(xml);
    $.cookie('lastBlocks', xmlText);
  }

  // Only store a cookie if a setting has changed. If we unconditionally stored
  // these, then updates to the default value would not be seen by users, as
  // the old defaults persisted in the cookies would override the new ones.
  if (isFontSizeChanged) $.cookie('fontSize', fontSize);
  if (isShowHeadingsChanged) $.cookie('showHeadings', showHeadings ? 1 : 0);
  if (isModelColorChanged) $.cookie('modelColor', modelColor);
  if (isShowWireframeChanged) $.cookie('showWireframe', showWireframe ? 1 : 0);
  if (isAxisChanged[0]) $.cookie('axisX', $('#axisX').prop('checked'));
  if (isAxisChanged[1]) $.cookie('axisY', $('#axisY').prop('checked'));
  if (isAxisChanged[2]) $.cookie('axisZ', $('#axisZ').prop('checked'));
  if (isPlaneChanged[0]) $.cookie('gridX', $('#gridX').prop('checked'));
  if (isPlaneChanged[1]) $.cookie('gridY', $('#gridY').prop('checked'));
  if (isPlaneChanged[2]) $.cookie('gridZ', $('#gridZ').prop('checked'));
  if (isNSecondsTillPreviewChanged) $.cookie('nSecondsTillPreview', nSecondsTillPreview);
  if (isGridSpacingChanged) $.cookie('gridSpacing', gridSpacing);
  if (isGridExtentChanged) $.cookie('gridExtent', gridExtent);
  $.cookie('leftWidth', $('#left').width());
  $.cookie('consoleHeight', $('#console').height());

  // Changes have been committed, so let's reset the dirty flags.
  isFontSizeChanged = false;
  isShowHeadingsChanged = false;
  isModelColorChanged = false;
  isShowWireframeChanged = false;
  isShowHeadingsChanged = false;
  isNSecondsTillPreviewChanged = false;
  isGridSpacingChanged = false;
  isGridExtentChanged = false;
  for (var d = 0; d < 3; ++d) {
    isAxisChanged[d] = false;
    isPlaneChanged[d] = false;
  }
}

$(document).ready(function() {
  $(window).load(function() {
    if ($.cookie('leftWidth')) {
      $('#left').width($.cookie('leftWidth'));
      resize();
    }

    if ($.cookie('consoleHeight')) {
      $('#console').height($.cookie('consoleHeight'));
      resize();
    }

    if ($.cookie('last')) {
      text_editor.setValue($.cookie('last'), -1);
    }

    if ($.cookie('fontSize')) {
      fontSize = parseInt($.cookie('fontSize'));
    } else {
      fontSize = 14;
    }
    text_editor.setFontSize(fontSize);
    $('#console')[0].style.fontSize = fontSize + 'px';

    if ($.cookie('showHeadings')) {
      showHeadings = parseInt($.cookie('showHeadings')) != 0;
    }
    $('#showHeadings').prop('checked', showHeadings);

    if ($.cookie('modelColor')) {
      modelColor = $.cookie('modelColor');
    }
    $('#modelColor').css('background-color', '#' + modelColor);

    $('#modelColor').ColorPicker({
      flat: false,
      color: modelColor,
      onSubmit: function(hsb, hex, rgb) {
        modelColor = hex;
        isModelColorChanged = true;
        $('#modelColor').css('background-color', '#' + modelColor);
        run(GeometryMode.SURFACE);
      }
    });

    if ($.cookie('showWireframe')) {
      showWireframe = parseInt($.cookie('showWireframe')) != 0;
    } else {
      showWireframe = false;
    }
    $('#showWireframe').prop('checked', showWireframe);

    if ($.cookie('gridExtent')) {
      gridExtent = parseFloat($.cookie('gridExtent'));
    }
    $('#gridSpacing').val(gridSpacing + '');

    if ($.cookie('gridSpacing')) {
      gridSpacing = parseFloat($.cookie('gridSpacing'));
    }
    $('#gridExtent').val(gridExtent + '');

    // JQuery Cookie stores cookies as strings, even booleans.
    if ($.cookie('axisX') === 'true') {
      $('#axisX').prop('checked', true);
      generateAxis(0);
    }

    if ($.cookie('axisY') === 'true') {
      $('#axisY').prop('checked', true);
      generateAxis(1);
    }

    if ($.cookie('axisZ') === 'true') {
      $('#axisZ').prop('checked', true);
      generateAxis(2);
    }

    if ($.cookie('gridX') === 'true') {
      $('#gridX').prop('checked', true);
      generatePlane(0);
    }

    if ($.cookie('gridY') === 'true') {
      $('#gridY').prop('checked', true);
      generatePlane(1);
    }

    if ($.cookie('gridZ') === 'true') {
      $('#gridZ').prop('checked', true);
      generatePlane(2);
    }

    if ($.cookie('nSecondsTillPreview')) {
      nSecondsTillPreview = parseFloat($.cookie('nSecondsTillPreview'));
    }

    $('#nSecondsTillPreview').val(nSecondsTillPreview + '');
    $('#nSecondsTillPreview').change(function () {
      nSecondsTillPreview = parseFloat($('#nSecondsTillPreview').val());
      isNSecondsTillPreviewChanged = true;
      text_editor.getSession().off('change', onEditorChange);
      if (preview) {
        clearTimeout(preview); 
      }
      preview = undefined;
      schedulePreview();
    });
    schedulePreview();
  });

  $(window).unload(function() {
    saveInCookies();
  });

  $('#clear').click(function() {
    log('');
    text_editor.focus();
  });

  $('#smaller').click(function() {
    isFontSizeChanged = true;
    fontSize -= 2;
    $('#menu input').each(function (i) {
      this.style.fontSize = (parseInt($(this).css('font-size')) - 1) + 'px';
    });
    text_editor.setFontSize(fontSize);
    $('#console')[0].style.fontSize = fontSize + 'px';
    text_editor.focus();
  });

  $('#bigger').click(function() {
    isFontSizeChanged = true;
    fontSize += 2;
    $('#menu input').each(function (i) {
      this.style.fontSize = (parseInt($(this).css('font-size')) + 1) + 'px';
    });
    text_editor.setFontSize(fontSize);
    $('#console')[0].style.fontSize = fontSize + 'px';
    text_editor.focus();
  });

  $('#fit').click(function() {
    fit();
    text_editor.focus();
  });

  $('input[type=radio][name=editorMode]').change(function() {
    var editorMode = $(this).val();
    if (editorMode == "Blocks") {
      $('#text_editor').hide();
      $('#blocksEditor').show();

      if (!workspace) {
        workspace = Blockly.inject('blocksCanvas', {toolbox: document.getElementById('toolbox')});
        workspace.addChangeListener(function() {
          var code = Blockly.Madeup.workspaceToCode(workspace);
          text_editor.setValue(code);
          log(code);
          //console.log(code);
        });
        if ($.cookie('lastBlocks')) {
          var xml = Blockly.Xml.textToDom($.cookie('lastBlocks'));
          Blockly.Xml.domToWorkspace(workspace, xml);
        }
      } else {
      }
    } else {
      $('#blocksEditor').hide();
      $('#text_editor').show();
    }
    resize();
    Blockly.fireUiEvent(window, 'resize');
  });

  $('#showHeadings').click(function() {
    isShowHeadingsChanged = true;
    showHeadings = this.checked;
    text_editor.focus();
    render();
  });

  var red = 0xB80000;
  var green = 0x006100;
  var blue = 0x0000FF;
  var colors = [red, green, blue];
  var axes = new Array(3);
  var planes = new Array(3);

  function generateAxis(d) {
    if (axes[d]) {
      removeAxis(d);
    }

    var geometry = new THREE.Geometry();
    var a = new THREE.Vector3(0, 0, 0);
    var b = new THREE.Vector3(0, 0, 0);
    a.setComponent(d, -gridExtent);
    b.setComponent(d, gridExtent);
    geometry.vertices.push(a);
    geometry.vertices.push(b);
    axes[d] = new THREE.Line(geometry, new THREE.LineBasicMaterial({
      color: colors[d],
      linewidth: 5
    }), THREE.LinePieces);
    scene.add(axes[d]);
    render();
  }

  function removeAxis(d) {
    if (axes[d]) {
      scene.remove(axes[d]);
      axes[d] = null;
      render();
    }
  }

  function toggleAxis(d) {
    isAxisChanged[d] = true;
    return function() {
      if (this.checked) {
        generateAxis(d);
      } else {
        removeAxis(d);
      }
    }
  }

  $('#axisX').click(toggleAxis(0));
  $('#axisY').click(toggleAxis(1));
  $('#axisZ').click(toggleAxis(2));
  
  function generatePlane(d) {
    if (planes[d]) {
      removePlane(d);
    }

    var geometry = new THREE.Geometry();
    for (var i = -gridExtent; i <= gridExtent; i += gridSpacing) {
      var a = new THREE.Vector3(0, 0, 0);
      var b = new THREE.Vector3(0, 0, 0);
      a.setComponent((d + 1) % 3, -gridExtent);
      b.setComponent((d + 1) % 3, gridExtent);
      a.setComponent((d + 2) % 3, i);
      b.setComponent((d + 2) % 3, i);
      geometry.vertices.push(a);
      geometry.vertices.push(b);

      a = new THREE.Vector3(0, 0, 0);
      b = new THREE.Vector3(0, 0, 0);
      a.setComponent((d + 1) % 3, i);
      b.setComponent((d + 1) % 3, i);
      a.setComponent((d + 2) % 3, -gridExtent);
      b.setComponent((d + 2) % 3, gridExtent);
      geometry.vertices.push(a);
      geometry.vertices.push(b);
    }

    planes[d] = new THREE.Line(geometry, new THREE.LineBasicMaterial({color: colors[d], linewidth: 1}), THREE.LinePieces);
    scene.add(planes[d]);
    render();
  }

  function removePlane(d) {
    if (planes[d]) {
      scene.remove(planes[d]);
      planes[d] = null;
      render();
    }
  }

  function togglePlane(d) {
    isPlaneChanged[d] = true;
    return function() {
      if (this.checked) {
        generatePlane(d);
      } else {
        removePlane(d);
      }
    }
  }

  $('#gridX').click(togglePlane(0));
  $('#gridY').click(togglePlane(1));
  $('#gridZ').click(togglePlane(2));

  $('#gridExtent').change(function() {
    isGridExtentChanged = true;
    gridExtent = parseFloat($(this).val());
    for (var d = 0; d < 3; ++d) {
      if (axes[d]) {
        generateAxis(d);
      }
      if (planes[d]) {
        generatePlane(d);
      }
    }
  });

  $('#gridSpacing').change(function() {
    isGridSpacingChanged = true;
    gridSpacing = parseFloat($(this).val());
    for (var d = 0; d < 3; ++d) {
      if (planes[d]) {
        generatePlane(d);
      }
    }
  });

  $('#autopreview').click(function() {
    if (this.checked) {
      $('#nSecondsTillPreview').prop('disabled', false);
      schedulePreview();
    } else {
      $('#nSecondsTillPreview').prop('disabled', true);
      text_editor.getSession().off('change', onEditorChange);
      if (preview) {
        clearTimeout(preview); 
      }
      preview = undefined;
    }
  });

  $('#showWireframe').click(function() {
    isShowWireframeChanged = true;
    showWireframe = this.checked;
    run(GeometryMode.SURFACE);
  });

  $('#toggleEditorPopup').click(function() {
    $('.popups').not('#editorPopup').hide();
    $('#editorPopup').css('top', $('#toggleEditorPopup').position().top + $('#toggleEditorPopup').innerHeight(true)) - 8; 
    $('#editorPopup').css('left', $('#toggleEditorPopup').position().left + 4); 
    $('#editorPopup').slideToggle('fast', function() {});
  });

  $('#toggleGridPopup').click(function() {
    $('.popups').not('#gridPopup').hide();
    $('#gridPopup').css('top', $('#toggleGridPopup').position().top + $('#toggleGridPopup').innerHeight(true)) - 8; 
    $('#gridPopup').css('left', $('#toggleGridPopup').position().left + 4); 
    $('#gridPopup').slideToggle('fast', function() {});
  });
  $('#toggleDisplayPopup').click(function() {
    $('.popups').not('#displayPopup').hide();
    $('#displayPopup').css('top', $('#toggleDisplayPopup').position().top + $('#toggleDisplayPopup').innerHeight(true)) - 8; 
    $('#displayPopup').css('left', $('#toggleDisplayPopup').position().left + 4); 
    $('#displayPopup').slideToggle('fast', function() {});
  });
  $('#download').click(function() {
    $('#source').val(getSource());
    $('#downloader').submit();
    text_editor.focus();
  });
  $('#run').click(function() {
    saveInCookies();
    run(GeometryMode.SURFACE);
    text_editor.focus();
  });

  $('#left').resizable({
    handles: "e",
    resize: function(event, ui) {
      resize();
      Blockly.fireUiEvent(window, 'resize');
      render();
    } 
  });

  $('#console').resizable({
    handles: "n",
    resize: function(event, ui) {
      resize();
      Blockly.fireUiEvent(window, 'resize');
      // Need this because console is relatively positioned.
      $('#console').css('top', '0px');
    } 
  });

  init();
  animate();
});

var onEditorChange = function(delta) {
  if (preview) {
    clearTimeout(preview); 
  }
  preview = setTimeout(function() {
    run(GeometryMode.PATH);
  }, nSecondsTillPreview * 1000);
}

var preview = undefined;
function schedulePreview() {
  text_editor.getSession().on('change', onEditorChange);
}

function getSource() {
  return text_editor.getValue();
}

var allGeometry = undefined;
function run(mode) {
  $.ajax({
    type: 'POST',
    url: 'interpret.php',
    data: JSON.stringify({source: getSource(), extension: 'json', mode: mode}),
    contentType: 'application/json; charset=utf-8',
    dataType: 'json',
    success: function(data) {
      var sansDebug = data['output'].replace(/^Debug:.*$\n/gm, '');
      if (sansDebug.length > 0) {
        console.log(sansDebug);
      }

      if (data['exit_status'] == 0) {
        for (var i = 0; i < meshes.length; ++i) {
          geoscene.remove(meshes[i]);
        }
        meshes = [];
        arrowShafts = [];

        log(sansDebug);
        
        if (mode == GeometryMode.SURFACE) {
          var loader = new THREE.JSONLoader();
          var model = loader.parse(JSON.parse(data['model']));
          var material = showWireframe ? new THREE.MeshBasicMaterial({color: parseInt(modelColor, 16), wireframe: showWireframe, wireframeLinewidth: 5})
                                       : new THREE.MeshLambertMaterial({color: parseInt(modelColor, 16), wireframe: showWireframe, wireframeLinewidth: 5});
          material.side = THREE.DoubleSide;
          //model.geometry.applyMatrix(new THREE.Matrix4().makeTranslation(0, 0, -10));
          meshes[0] = new THREE.Mesh(model.geometry, material);
          allGeometry = model.geometry;
        } else {
          var paths = JSON.parse(data['model']);
          allGeometry = new THREE.Geometry();
          var dotsGeometry = new THREE.Geometry();

          for (var pi = 0; pi < paths.length; ++pi) {
            var geometry = new THREE.Geometry();
            for (var i = 0; i < paths[pi].vertices.length; ++i) {
              var v = new THREE.Vector3(paths[pi].vertices[i][0], paths[pi].vertices[i][1], paths[pi].vertices[i][2]);
              geometry.vertices.push(v);
              dotsGeometry.vertices.push(v);
              allGeometry.vertices.push(v);
            }
            meshes[meshes.length] = new THREE.Line(geometry, new THREE.LineBasicMaterial({
              color: 0x6666FF,
              linewidth: 3
            }));

            var nvertices = paths[pi].vertices.length;
            if (nvertices > 0) {
              var m = paths[pi].orientation;
              
              var g2 = new THREE.Geometry();

              g2.vertices.push(
                new THREE.Vector3(-0.5, 0,  0),
                new THREE.Vector3( 0.5, 0,  0),
                new THREE.Vector3( 0, 0, -1),
                new THREE.Vector3( 0, 0.3, -0.2)
              );

              g2.faces.push(
                new THREE.Face3(1, 0, 2),
                new THREE.Face3(0, 1, 3),
                new THREE.Face3(3, 2, 0),
                new THREE.Face3(1, 2, 3)
              );

              var mm = new THREE.Matrix4().set(m[0], m[1], m[2], m[3], m[4], m[5], m[6], m[7], m[8], m[9], m[10], m[11], m[12], m[13], m[14], m[15]);
              var mmm = new THREE.Matrix4().getInverse(mm);
              g2.applyMatrix(mmm);
              var offset = new THREE.Vector3(paths[pi].vertices[paths[pi].vertices.length - 1][0], paths[pi].vertices[paths[pi].vertices.length - 1][1], paths[pi].vertices[paths[pi].vertices.length - 1][2]);
              g2.applyMatrix(new THREE.Matrix4().makeTranslation(offset.x, offset.y, offset.z));

              g2.computeFaceNormals();
              allGeometry.vertices = allGeometry.vertices.concat(g2.vertices);

              meshes[meshes.length] = new THREE.Mesh(g2, new THREE.MeshLambertMaterial({
                color: 0x0000ff,
              }));
              pointerScene.add(meshes[meshes.length - 1]);
            }
          }

          meshes[meshes.length] = new THREE.PointCloud(dotsGeometry, new THREE.PointCloudMaterial({
            color: 0x000000,
            size: 8,
            sizeAttenuation: false
          }));

        }

        /* if (isAutofit) { */
          /* allGeometry.computeBoundingBox(); */
          /* allGeometry.center(); */
          /* var bounds = allGeometry.boundingBox; */
          /* console.log(camera.fov + ' ' + camera.position.z + ' ' + bounds.max.y); */
          /* var verticalFitZ = bounds.max.y / Math.tan(camera.fov * Math.PI / 180.0 * 0.5); */
          /* var fovX = 2 * Math.atan(Math.tan(camera.fov * Math.PI / 180.0 * 0.5) * camera.aspect); */
          /* var horizontalFitZ = bounds.max.x / Math.tan(fovX * 0.5); */
          /* console.log("max z " + bounds.max.z); */
          /* console.log("min z " + bounds.min.z); */
          /* camera.position.z = bounds.max.z + (verticalFitZ > horizontalFitZ ? verticalFitZ : horizontalFitZ); */
          /* camera.updateProjectionMatrix(); */

          /* // Update orbit controls so that it will reset to the last fit -- instead of the camera's initial settings. */
          /* controls.position0 = camera.position.clone(); */
        /* } */

        for (var mi = 0; mi < meshes.length; ++mi) {
          /* meshes[mi].geometry.mergeVertices(); */
          /* meshes[mi].geometry.computeFaceNormals(); */
          /* meshes[mi].geometry.computeVertexNormals(); */
          geoscene.add(meshes[mi]);
        }
        render();
      } else if (data['exit_status'] == 22) {
        log(data['output'] + '\nYour model was taking a long time to build. It felt like it was never going to finish! So, I stopped trying. Sorry.');
      } else {
        log(sansDebug);
      }
    },
    failure: function(errorMessage) {
      console.log('Failure. :(');
    }
  });
}

function fit() {
  if (allGeometry === undefined) {
    return;
  }

  allGeometry.computeBoundingBox();
  /* allGeometry.center(); */
  var bounds = allGeometry.boundingBox;
  var centroid = bounds.center();

  var xform = new THREE.Matrix4().makeTranslation(-centroid.x, -centroid.y, -centroid.z);
  geoscene.matrix = xform;

  var verticalFitZ = bounds.max.y / Math.tan(camera.fov * Math.PI / 180.0 * 0.5);
  var fovX = 2 * Math.atan(Math.tan(camera.fov * Math.PI / 180.0 * 0.5) * camera.aspect);
  var horizontalFitZ = bounds.max.x / Math.tan(fovX * 0.5);
  controls.reset(); 
  camera.position.z = bounds.max.z + (verticalFitZ > horizontalFitZ ? verticalFitZ : horizontalFitZ);
  camera.updateProjectionMatrix();

  // Update orbit controls so that it will reset to the last fit -- instead of the camera's initial settings.
  /* controls.position0 = camera.position.clone(); */
}

function log(message) {
  // $1 is the whole source span. $2 is the start. $3 is the end.
  var linkMessage = message.replace(/^((\d+)\((\d+)(?:-(\d+))?\))/gm, function(match, full, startLine, startIndex, stopIndex) {
    return '<div style="color: #FF9999; display: inline;">Error on <a style="text-decoration: underline;" onclick="javascript:highlight(' + startIndex + ', ' + stopIndex + ')" class="srclink">line ' + startLine + /*':' + startIndex + ':' + stopIndex +*/ '</a></div>';
  });

  $('#console #message').html(linkMessage.replace(/\n/g, '<br/>'));
}

function resize() {
  var width = window.innerWidth - $('#left').width();
  var height = window.innerHeight;
  renderer.setSize(width, height);
  camera.aspect = width / height;
  camera.updateProjectionMatrix();
  $("#text_editor").height($(document).height() - $('#menu').height() - $('#console').height());
  $("#blocksEditor").height($(document).height() - $('#menu').height() - $('#console').height());

  var blocklyArea = document.getElementById('blocksEditor');
  var blocklyDiv = document.getElementById('blocksCanvas');
  // Position blocklyDiv over blocklyArea.
  blocklyDiv.style.left = $('#blocksEditor').position().left + 'px';
  blocklyDiv.style.top = $('#blocksEditor').position().top + 'px';
  blocklyDiv.style.height = blocklyArea.offsetHeight + 'px';
  $("#blocksCanvas").width(blocklyArea.offsetWidth);

  text_editor.resize();
}

function highlight(startIndex, stopIndex) {
  var doc = text_editor.getSession().getDocument();
  var lines = doc.getAllLines();
  //console.log(lines);

  if (stopIndex === undefined) {
    stopIndex = startIndex;
  }

  var indexToRowColumn = function (targetIndex) {
    var row = 0;
    var indexSoFar = 0; 
    
    while (row < lines.length && indexSoFar + lines[row].length < targetIndex) {
      indexSoFar += lines[row].length;
      ++indexSoFar; // linebreak
      ++row;
    }

    var column = targetIndex - indexSoFar;

    return {row: row, column: column};
  }
    
  var start = indexToRowColumn(startIndex);
  var stop = indexToRowColumn(stopIndex);

  text_editor.getSelection().setSelectionRange(new Range(start.row, start.column, stop.row, stop.column + 1));
}

function init() {
  THREE.Camera.prototype.getWorldRight = function () {
    var quaternion = new THREE.Quaternion();
    return function (optionalTarget) {
      var result = optionalTarget || new THREE.Vector3();
      this.getWorldQuaternion(quaternion);
      return result.set(1, 0, 0).applyQuaternion(quaternion);
    }
  }();

  camera = new THREE.PerspectiveCamera(45.0, 1.0, 0.1, 10000.0);
  camera.position.z = 30;

  var glcanvas = $("#glcanvas");
  renderer = new THREE.WebGLRenderer();
  renderer.setClearColor(0xFFFFFF, 1);
  document.getElementById("glcanvas").appendChild(renderer.domElement);

  controls = new THREE.OrbitControls(camera, renderer.domElement);
  //controls.autoRotate = true;
  controls.addEventListener('change', function() {
    render();
  });

  /*
  var vector = new THREE.Vector3( 0, 0, -1 );
  vector.applyQuaternion(camera.quaternion);
  alert(vector.toArray());
  */

  window.addEventListener('resize', resize);
  resize();

  pointerScene = new THREE.Scene();
  geoscene = new THREE.Scene();
  geoscene.matrixAutoUpdate = false;
  scene = new THREE.Scene();
  geoscene.add(pointerScene);
  scene.add(geoscene);
  scene.add(camera);

  var radius = 50,
      segments = 16,
      rings = 16;

  // create a point light
  var pointLight = new THREE.PointLight(0xFFFFFF);

  // set its position
  pointLight.position.x = 10;
  pointLight.position.y = 50;
  pointLight.position.z = 130;

  // add to the scene
  camera.add(pointLight);

  render();
}

function animate() {
  requestAnimationFrame(animate);
  controls.update();
}

function render() {
  renderer.render(scene, camera);
}
