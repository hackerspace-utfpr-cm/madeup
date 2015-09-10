/**
 * @license
 * Visual Blocks Language
 *
 * Copyright 2012 Google Inc.
 * https://developers.google.com/blockly/
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/**
 * @fileoverview Helper functions for generating Python for blocks.
 * @author fraser@google.com (Neil Fraser)
 */
'use strict';

goog.provide('Blockly.Madeup');

goog.require('Blockly.Generator');


/**
 * Madeup code generator.
 * @type !Blockly.Generator
 */
Blockly.Madeup = new Blockly.Generator('Madeup');

/**
 * List of illegal variable names.
 * This is not intended to be a security feature.  Blockly is 100% client-side,
 * so bypassing this list is trivial.  This is intended to prevent users from
 * accidentally clobbering a built-in object or function.
 * @private
 */
Blockly.Madeup.addReservedWords('');
    // import keyword
    // print ','.join(keyword.kwlist)
    // http://docs.python.org/reference/lexical_analysis.html#keywords
    //'and,as,assert,break,class,continue,def,del,elif,else,except,exec,finally,for,from,global,if,import,in,is,lambda,not,or,pass,print,raise,return,try,while,with,yield,' +
    //http://docs.python.org/library/constants.html
    //'True,False,None,NotImplemented,Ellipsis,__debug__,quit,exit,copyright,license,credits,' +
    // http://docs.python.org/library/functions.html
    //'abs,divmod,input,open,staticmethod,all,enumerate,int,ord,str,any,eval,isinstance,pow,sum,basestring,execfile,issubclass,print,super,bin,file,iter,property,tuple,bool,filter,len,range,type,bytearray,float,list,raw_input,unichr,callable,format,locals,reduce,unicode,chr,frozenset,long,reload,vars,classmethod,getattr,map,repr,xrange,cmp,globals,max,reversed,zip,compile,hasattr,memoryview,round,__import__,complex,hash,min,set,apply,delattr,help,next,setattr,buffer,dict,hex,object,slice,coerce,dir,id,oct,sorted,intern');

/**
 * Order of operation ENUMs.
 * http://docs.python.org/reference/expressions.html#summary
 */
Blockly.Madeup.INDENT = '  ';            // 0 "" ...
Blockly.Madeup.ORDER_ATOMIC = 0;            // 0 "" ...
//Blockly.Madeup.ORDER_COLLECTION = 1;        // tuples, lists, dictionaries
//Blockly.Madeup.ORDER_STRING_CONVERSION = 1; // `expression...`
//Blockly.Madeup.ORDER_MEMBER = 2;            // . []
Blockly.Madeup.ORDER_FUNCTION_CALL = 2;     // ()
Blockly.Madeup.ORDER_EXPONENTIATION = 3;    // **
//Blockly.Madeup.ORDER_BITWISE_NOT = 4;       // ~
Blockly.Madeup.ORDER_UNARY_NEGATION = 4;        // + -
Blockly.Madeup.ORDER_MULTIPLICATIVE = 5;    // * / // %
Blockly.Madeup.ORDER_ADDITIVE = 6;          // + -
//Blockly.Madeup.ORDER_BITWISE_SHIFT = 7;     // << >>
//Blockly.Madeup.ORDER_BITWISE_AND = 8;       // &
//Blockly.Madeup.ORDER_BITWISE_XOR = 9;       // ^
//Blockly.Madeup.ORDER_BITWISE_OR = 10;       // |
Blockly.Madeup.ORDER_RELATIONAL = 11;       // in, not in, is, is not,
                                            //     <, <=, >, >=, <>, !=, ==
Blockly.Madeup.ORDER_EQUALITY = 12;
Blockly.Madeup.ORDER_LOGICAL_NOT = 13;      // not
Blockly.Madeup.ORDER_LOGICAL_AND = 14;      // and
Blockly.Madeup.ORDER_LOGICAL_OR = 15;       // or
Blockly.Madeup.ORDER_CONDITIONAL = 16;      // if else
//Blockly.Madeup.ORDER_LAMBDA = 17;           // lambda
Blockly.Madeup.ORDER_NONE = 99;             // (...)
Blockly.Madeup.ORDER_UNARY_NEGATION_CONTEXT = 100;        // + -

/**
 * Initialise the database of variable names.
 * @param {!Blockly.Workspace} workspace Workspace to generate code from.
 */
Blockly.Madeup.init = function(workspace) {
  // Create a dictionary of definitions to be printed before the code.
  Blockly.Madeup.definitions_ = Object.create(null);
  // Create a dictionary mapping desired function names in definitions_
  // to actual function names (to avoid collisions with user functions).
  Blockly.Madeup.functionNames_ = Object.create(null);

  if (!Blockly.Madeup.variableDB_) {
    Blockly.Madeup.variableDB_ =
        new Blockly.Names(Blockly.Madeup.RESERVED_WORDS_);
  } else {
    Blockly.Madeup.variableDB_.reset();
  }

  //var defvars = [];
  //var variables = Blockly.Variables.allVariables(workspace);
  //for (var x = 0; x < variables.length; x++) {
  //  defvars[x] = Blockly.Madeup.variableDB_.getName(variables[x],
  //      Blockly.Variables.NAME_TYPE) + ' = None';
  //}
  //Blockly.Madeup.definitions_['variables'] = defvars.join('\n');
};

/**
 * Prepend the generated code with the variable definitions.
 * @param {string} code Generated code.
 * @return {string} Completed code.
 */
Blockly.Madeup.finish = function(code) {
  // Convert the definitions dictionary into a list.
  var imports = [];
  var definitions = [];
  for (var name in Blockly.Madeup.definitions_) {
    var def = Blockly.Madeup.definitions_[name];
    if (def.match(/^(from\s+\S+\s+)?import\s+\S+/)) {
      imports.push(def);
    } else {
      definitions.push(def);
    }
  }
  var allDefs = imports.join('\n') + '\n\n' + definitions.join('\n\n');
  return allDefs.replace(/\n\n+/g, '\n\n').replace(/\n*$/, '\n\n\n') + code;
};

/**
 * Naked values are top-level blocks with outputs that aren't plugged into
 * anything.
 * @param {string} line Line of generated code.
 * @return {string} Legal line of code.
 */
Blockly.Madeup.scrubNakedValue = function(line) {
  return line + '\n';
};

/**
 * Common tasks for generating Python from blocks.
 * Handles comments for the specified block and any connected value blocks.
 * Calls any statements following this block.
 * @param {!Blockly.Block} block The current block.
 * @param {string} code The Python code created for this block.
 * @return {string} Python code with comments and subsequent blocks added.
 * @private
 */
Blockly.Madeup.scrub_ = function(block, code) {
  var commentCode = '';
  // Only collect comments for blocks that aren't inline.
  if (!block.outputConnection || !block.outputConnection.targetConnection) {
    // Collect comment for this block.
    var comment = block.getCommentText();
    if (comment) {
      commentCode += Blockly.Madeup.prefixLines(comment, '# ') + '\n';
    }
    // Collect comments for all value arguments.
    // Don't collect comments for nested statements.
    for (var x = 0; x < block.inputList.length; x++) {
      if (block.inputList[x].type == Blockly.INPUT_VALUE) {
        var childBlock = block.inputList[x].connection.targetBlock();
        if (childBlock) {
          var comment = Blockly.Madeup.allNestedComments(childBlock);
          if (comment) {
            commentCode += Blockly.Madeup.prefixLines(comment, '# ');
          }
        }
      }
    }
  }
  var nextBlock = block.nextConnection && block.nextConnection.targetBlock();
  var nextCode = Blockly.Madeup.blockToCode(nextBlock);
  return commentCode + code + nextCode;
};

Blockly.Madeup['madeup_io_print'] = function(block) {
  var value_message = Blockly.Madeup.valueToCode(block, 'MESSAGE', Blockly.Madeup.ORDER_NONE);
  var code = 'print ' + value_message + '\n';
  return code;
};

Blockly.Madeup['madeup_io_debug'] = function(block) {
  var value_message = Blockly.Madeup.valueToCode(block, 'MESSAGE', Blockly.Madeup.ORDER_NONE);
  var code = 'debug ' + value_message + '\n';
  return code;
};

Blockly.Madeup['madeup_io_where'] = function(block) {
  var code = 'where\n';
  return code;
};

Blockly.Madeup['madeup_movement_moveto'] = function(block) {
  var value_x = Blockly.Madeup.valueToCode(block, 'X', Blockly.Madeup.ORDER_FUNCTION_CALL);
  var value_y = Blockly.Madeup.valueToCode(block, 'Y', Blockly.Madeup.ORDER_FUNCTION_CALL);
  var value_z = Blockly.Madeup.valueToCode(block, 'Z', Blockly.Madeup.ORDER_FUNCTION_CALL);
  var code = 'moveto ' + value_x + ', ' + value_y + ', ' + value_z + '\n';
  return code;
};

Blockly.Madeup['madeup_math_random'] = function(block) {
  var value_min = Blockly.Madeup.valueToCode(block, 'MIN', Blockly.Madeup.ORDER_NONE);
  var value_max = Blockly.Madeup.valueToCode(block, 'MAX', Blockly.Madeup.ORDER_NONE);
  var code = 'random ' + value_min + ', ' + value_max;
  return [code, Blockly.Madeup.ORDER_NONE];
};

Blockly.Madeup['madeup_movement_move'] = function(block) {
  var value_distance = Blockly.Madeup.valueToCode(block, 'DISTANCE', Blockly.Madeup.ORDER_NONE);
  var code = 'move ' + value_distance + '\n';
  return code;
};

Blockly.Madeup['madeup_math_integer'] = function(block) {
  var code = block.getFieldValue('INTEGER');
  return [code, Blockly.Madeup.ORDER_ATOMIC];
};

Blockly.Madeup['madeup_nothing'] = function(block) {
  var code = 'nothing';
  return [code, Blockly.Madeup.ORDER_ATOMIC];
};

Blockly.Madeup['madeup_math_real'] = function(block) {
  var code = block.getFieldValue('REAL');
  return [code, Blockly.Madeup.ORDER_ATOMIC];
};

Blockly.Madeup['madeup_loop_repeat'] = function(block) {
  var value_count = Blockly.Madeup.valueToCode(block, 'COUNT', Blockly.Madeup.ORDER_NONE);
  var statements_block = Blockly.Madeup.statementToCode(block, 'BLOCK');
  var code = 'repeat ' + value_count + '\n' + statements_block + 'end\n';
  return code;
};

Blockly.Madeup['madeup_loop_repeatwich'] = function(block) {
  var value_count = Blockly.Madeup.valueToCode(block, 'COUNT', Blockly.Madeup.ORDER_NONE);
  var surrounder_block = Blockly.Madeup.statementToCode(block, 'SURROUNDER');
  var surroundee_block = Blockly.Madeup.statementToCode(block, 'SURROUNDEE');
  var code = 'repeatwich ' + value_count + '\n' + surrounder_block + '\naround\n' + surroundee_block + 'end\n';
  return code;
};

Blockly.Madeup['madeup_movement_turn'] = function(block) {
  var value_degrees = Blockly.Madeup.valueToCode(block, 'DEGREES', Blockly.Madeup.ORDER_NONE);
  var dropdown_type = block.getFieldValue('TYPE');
  var code = dropdown_type.toLowerCase() + ' ' + value_degrees + '\n';
  return code;
};

Blockly.Madeup['madeup_math_sign'] = function(block) {
  var value_x = Blockly.Madeup.valueToCode(block, 'X', Blockly.Madeup.ORDER_NONE);
  var code = 'sign ' + value_x;
  return [code, Blockly.Madeup.ORDER_ATOMIC];
};

Blockly.Madeup['madeup_math_abs'] = function(block) {
  var value_x = Blockly.Madeup.valueToCode(block, 'X', Blockly.Madeup.ORDER_NONE);
  var code = '|' + value_x + '|';
  return [code, Blockly.Madeup.ORDER_ATOMIC];
};

Blockly.Madeup['madeup_math_sincostan'] = function(block) {
  var value_degrees = Blockly.Madeup.valueToCode(block, 'DEGREES', Blockly.Madeup.ORDER_NONE);
  var dropdown_f = block.getFieldValue('F');
  var code = dropdown_f.toLowerCase() + ' ' + value_degrees;
  return [code, Blockly.Madeup.ORDER_FUNCTION_CALL];
};

Blockly.Madeup['madeup_math_inverse_sincostan'] = function(block) {
  var value_ratio = Blockly.Madeup.valueToCode(block, 'RATIO', Blockly.Madeup.ORDER_NONE);
  var dropdown_f = block.getFieldValue('F');
  var code = dropdown_f.toLowerCase() + ' ' + value_ratio;
  return [code, Blockly.Madeup.ORDER_ATOMIC];
};

Blockly.Madeup['madeup_logic_junction'] = function(block) {
  var dropdown_f = block.getFieldValue('F');
  var precedence;
  if (dropdown_f == 'and') {
    precedence = Blockly.Madeup.ORDER_LOGICAL_AND;
  } else {
    precedence = Blockly.Madeup.ORDER_LOGICAL_OR;
  }
  var value_a = Blockly.Madeup.valueToCode(block, 'A', precedence);
  var value_b = Blockly.Madeup.valueToCode(block, 'B', precedence);
  var code = value_a + ' ' + dropdown_f.toLowerCase() + ' ' + value_b;
  return [code, precedence];
};

Blockly.Madeup['madeup_logic_not'] = function(block) {
  var value_a = Blockly.Madeup.valueToCode(block, 'A', Blockly.Madeup.ORDER_LOGICAL_NOT);
  var code = 'not ' + value_a;
  return [code, Blockly.Madeup.ORDER_LOGICAL_NOT];
};

Blockly.Madeup['madeup_movement_center'] = function(block) {
  var code = 'center\n';
  return code;
};

Blockly.Madeup['madeup_movement_identity'] = function(block) {
  var code = 'identity\n';
  return code;
};

Blockly.Madeup['madeup_movement_reframe'] = function(block) {
  var code = 'reframe\n';
  return code;
};

Blockly.Madeup['madeup_movement_push'] = function(block) {
  var code = 'push\n';
  return code;
};

Blockly.Madeup['madeup_movement_pop'] = function(block) {
  var code = 'pop\n';
  return code;
};

Blockly.Madeup['madeup_movement_reverse'] = function(block) {
  var code = 'reverse\n';
  return code;
};

Blockly.Madeup['madeup_movement_where'] = function(block) {
  var code = 'where\n';
  return code;
};

Blockly.Madeup['madeup_generate_dowel'] = function(block) {
  var value_maxBend = Blockly.Madeup.valueToCode(block, 'MAXBEND', Blockly.Madeup.ORDER_NONE);
  var code = 'dowel maxBend:' + value_maxBend + '\n';
  return code;
};

Blockly.Madeup['madeup_generate_tube'] = function(block) {
  var value_maxBend = Blockly.Madeup.valueToCode(block, 'MAXBEND', Blockly.Madeup.ORDER_NONE);
  var code = 'tube maxBend:' + value_maxBend + '\n';
  return code;
};

Blockly.Madeup['madeup_generate_dot'] = function(block) {
  var code = 'dot\n';
  return code;
};

Blockly.Madeup['madeup_generate_forget'] = function(block) {
  var code = 'forget\n';
  return code;
};

Blockly.Madeup['madeup_generate_box'] = function(block) {
  var code = 'box\n';
  return code;
};

Blockly.Madeup['madeup_logic_boolean'] = function(block) {
  var dropdown_boolean = block.getFieldValue('BOOLEAN');
  var code = dropdown_boolean.toLowerCase();
  return [code, Blockly.Madeup.ORDER_ATOMIC];
};

Blockly.Madeup['madeup_generate_extrude'] = function(block) {
  var value_x = Blockly.Madeup.valueToCode(block, 'X', Blockly.Madeup.ORDER_NONE);
  var value_y = Blockly.Madeup.valueToCode(block, 'Y', Blockly.Madeup.ORDER_NONE);
  var value_z = Blockly.Madeup.valueToCode(block, 'Z', Blockly.Madeup.ORDER_NONE);
  var value_length = Blockly.Madeup.valueToCode(block, 'LENGTH', Blockly.Madeup.ORDER_ATOMIC);
  var code = 'extrude ' + value_x + ', ' + value_y + ', ' + value_z + ', ' + value_length + '\n';
  return code;
};

Blockly.Madeup['madeup_generate_revolve'] = function(block) {
  var value_x = Blockly.Madeup.valueToCode(block, 'X', Blockly.Madeup.ORDER_NONE);
  var value_y = Blockly.Madeup.valueToCode(block, 'Y', Blockly.Madeup.ORDER_NONE);
  var value_z = Blockly.Madeup.valueToCode(block, 'Z', Blockly.Madeup.ORDER_NONE);
  var value_degrees = Blockly.Madeup.valueToCode(block, 'DEGREES', Blockly.Madeup.ORDER_ATOMIC);
  var code = 'revolve ' + value_x + ', ' + value_y + ', ' + value_z + ', ' + value_degrees + '\n';
  return code;
};

Blockly.Madeup['madeup_math_binary_arithmetic_operator'] = function(block) {
  var dropdown_operator = block.getFieldValue('OPERATOR');

  var precedence;
  if (dropdown_operator == '+' || dropdown_operator == '-') {
    precedence = Blockly.Madeup.ORDER_ADDITIVE;
  } else if (dropdown_operator == '*' || dropdown_operator == '/' || dropdown_operator == '//' || dropdown_operator == '%') {
    precedence = Blockly.Madeup.ORDER_MULTIPLICATIVE;
  } else {
    precedence = Blockly.Madeup.ORDER_EXPONENTIATION;
  } 
  console.log(dropdown_operator + ': ' + precedence);

  var value_a = Blockly.Madeup.valueToCode(block, 'A', precedence);
  var value_b = Blockly.Madeup.valueToCode(block, 'B', precedence);
  var code = value_a + ' ' + dropdown_operator + ' ' + value_b;
  return [code, precedence];
};

Blockly.Madeup['madeup_math_unary_operator'] = function(block) {
  var value_a = Blockly.Madeup.valueToCode(block, 'A', Blockly.Madeup.ORDER_UNARY_NEGATION);
  var code = '-' + value_a;
  return [code, Blockly.Madeup.ORDER_UNARY_NEGATION_CONTEXT];
};

Blockly.Madeup['madeup_math_minmax'] = function(block) {
  var value_a = Blockly.Madeup.valueToCode(block, 'A', Blockly.Madeup.ORDER_NONE);
  var dropdown_f = block.getFieldValue('F');
  var value_b = Blockly.Madeup.valueToCode(block, 'B', Blockly.Madeup.ORDER_NONE);
  var code = dropdown_f.toLowerCase() + ' ' + value_a + ', ' + value_b;
  return [code, Blockly.Madeup.ORDER_ATOMIC];
};

Blockly.Madeup['madeup_generate_surface'] = function(block) {
  var value_columns = Blockly.Madeup.valueToCode(block, 'COLUMNS', Blockly.Madeup.ORDER_NONE);
  var value_rows = Blockly.Madeup.valueToCode(block, 'ROWS', Blockly.Madeup.ORDER_NONE);
  var code = 'surface ' + value_columns + ', ' + value_rows + '\n';
  return code;
};

Blockly.Madeup['madeup_math_log'] = function(block) {
  var value_base = Blockly.Madeup.valueToCode(block, 'BASE', Blockly.Madeup.ORDER_NONE);
  var value_x = Blockly.Madeup.valueToCode(block, 'X', Blockly.Madeup.ORDER_NONE);
  var code = 'log ' + value_base + ', ' + value_x;
  return [code, Blockly.Madeup.ORDER_NONE];
};

Blockly.Madeup['madeup_math_relational_operator'] = function(block) {
  var dropdown_operator = block.getFieldValue('OPERATOR');
  var precedence;
  if (dropdown_operator == '==' || dropdown_operator == '!=') {
    precedence = Blockly.Madeup.ORDER_EQUALITY;
  } else {
    precedence = Blockly.Madeup.ORDER_RELATIONAL;
  }
  var value_a = Blockly.Madeup.valueToCode(block, 'A', precedence);
  var value_b = Blockly.Madeup.valueToCode(block, 'B', precedence);
  var code = value_a + ' ' + dropdown_operator + ' ' + value_b;
  return [code, precedence];
};

Blockly.Madeup['madeup_logic_if_expr'] = function(block) {
  var value_condition = Blockly.Madeup.valueToCode(block, 'CONDITION', Blockly.Madeup.ORDER_NONE);
  var value_then = Blockly.Madeup.valueToCode(block, 'THEN', Blockly.Madeup.ORDER_NONE);
  var value_else = Blockly.Madeup.valueToCode(block, 'ELSE', Blockly.Madeup.ORDER_NONE);
  var code = 'if ' + value_condition + ' then ' + value_then + ' else ' + value_else;
  return [code, Blockly.Madeup.ORDER_CONDITIONAL];
};

Blockly.Madeup['madeup_logic_if_statement'] = function(block) {
  var value_condition = Blockly.Madeup.valueToCode(block, 'CONDITION', Blockly.Madeup.ORDER_NONE);
  var statements_then = Blockly.Madeup.statementToCode(block, 'THEN');
  var code = 'if ' + value_condition + '\n' + statements_then + 'end\n';
  return code;
};

Blockly.Madeup['madeup_logic_if_else_statement'] = function(block) {
  var value_condition = Blockly.Madeup.valueToCode(block, 'CONDITION', Blockly.Madeup.ORDER_NONE);
  var statements_then = Blockly.Madeup.statementToCode(block, 'THEN');
  var statements_else = Blockly.Madeup.statementToCode(block, 'ELSE');
  var code = 'if ' + value_condition + '\n' + statements_then + 'else\n' + statements_else + 'end\n';
  return code;
};

Blockly.Madeup['madeup_movement_rotate'] = function(block) {
  var value_x = Blockly.Madeup.valueToCode(block, 'X', Blockly.Madeup.ORDER_NONE);
  var value_y = Blockly.Madeup.valueToCode(block, 'Y', Blockly.Madeup.ORDER_NONE);
  var value_z = Blockly.Madeup.valueToCode(block, 'Z', Blockly.Madeup.ORDER_NONE);
  var value_degrees = Blockly.Madeup.valueToCode(block, 'DEGREES', Blockly.Madeup.ORDER_NONE);
  var code = 'rotate ' + value_x + ', ' + value_y + ', ' + value_z + ', ' + value_degrees + '\n';
  return code;
};

Blockly.Madeup['madeup_movement_scale'] = function(block) {
  var value_x = Blockly.Madeup.valueToCode(block, 'X', Blockly.Madeup.ORDER_NONE);
  var value_y = Blockly.Madeup.valueToCode(block, 'Y', Blockly.Madeup.ORDER_NONE);
  var value_z = Blockly.Madeup.valueToCode(block, 'Z', Blockly.Madeup.ORDER_NONE);
  var code = 'scale ' + value_x + ', ' + value_y + ', ' + value_z + '\n';
  return code;
};

Blockly.Madeup['madeup_movement_translate'] = function(block) {
  var value_x = Blockly.Madeup.valueToCode(block, 'X', Blockly.Madeup.ORDER_NONE);
  var value_y = Blockly.Madeup.valueToCode(block, 'Y', Blockly.Madeup.ORDER_NONE);
  var value_z = Blockly.Madeup.valueToCode(block, 'Z', Blockly.Madeup.ORDER_NONE);
  var code = 'translate ' + value_x + ', ' + value_y + ', ' + value_z + '\n';
  return code;
};

Blockly.Madeup['madeup_loop_while'] = function(block) {
  var value_condition = Blockly.Madeup.valueToCode(block, 'CONDITION', Blockly.Madeup.ORDER_NONE);
  var statements_body = Blockly.Madeup.statementToCode(block, 'BODY');
  var code = 'while ' + value_condition + '\n' + statements_body + 'end\n';
  return code;
};

Blockly.Madeup['madeup_loop_for_to'] = function(block) {
  var value_stop = Blockly.Madeup.valueToCode(block, 'STOP', Blockly.Madeup.ORDER_NONE);
  var variable_iterator = Blockly.Madeup.variableDB_.getName(block.getFieldValue('ITERATOR'), Blockly.Variables.NAME_TYPE);
  var statements_body = Blockly.Madeup.statementToCode(block, 'BODY');
  var code = 'for ' + variable_iterator + ' to ' + value_stop + '\n' + statements_body + 'end\n';
  return code;
};

Blockly.Madeup['madeup_loop_for_to_by'] = function(block) {
  var value_stop = Blockly.Madeup.valueToCode(block, 'STOP', Blockly.Madeup.ORDER_NONE);
  var value_by = Blockly.Madeup.valueToCode(block, 'BY', Blockly.Madeup.ORDER_NONE);
  var variable_iterator = Blockly.Madeup.variableDB_.getName(block.getFieldValue('ITERATOR'), Blockly.Variables.NAME_TYPE);
  var statements_body = Blockly.Madeup.statementToCode(block, 'BODY');
  var code = 'for ' + variable_iterator + ' to ' + value_stop + ' by ' + value_by + '\n' + statements_body + 'end\n';
  return code;
};

Blockly.Madeup['madeup_loop_for_through'] = function(block) {
  var value_stop = Blockly.Madeup.valueToCode(block, 'STOP', Blockly.Madeup.ORDER_NONE);
  var variable_iterator = Blockly.Madeup.variableDB_.getName(block.getFieldValue('ITERATOR'), Blockly.Variables.NAME_TYPE);
  var statements_body = Blockly.Madeup.statementToCode(block, 'BODY');
  var code = 'for ' + variable_iterator + ' through ' + value_stop + '\n' + statements_body + 'end\n';
  return code;
};

Blockly.Madeup['madeup_loop_for_through_by'] = function(block) {
  var value_stop = Blockly.Madeup.valueToCode(block, 'STOP', Blockly.Madeup.ORDER_NONE);
  var value_by = Blockly.Madeup.valueToCode(block, 'BY', Blockly.Madeup.ORDER_NONE);
  var variable_iterator = Blockly.Madeup.variableDB_.getName(block.getFieldValue('ITERATOR'), Blockly.Variables.NAME_TYPE);
  var statements_body = Blockly.Madeup.statementToCode(block, 'BODY');
  var code = 'for ' + variable_iterator + ' through ' + value_stop + ' by ' + value_by + '\n' + statements_body + 'end\n';
  return code;
};

Blockly.Madeup['madeup_loop_for_in'] = function(block) {
  var value_start = Blockly.Madeup.valueToCode(block, 'START', Blockly.Madeup.ORDER_NONE);
  var variable_iterator = Blockly.Madeup.variableDB_.getName(block.getFieldValue('ITERATOR'), Blockly.Variables.NAME_TYPE);
  var value_stop = Blockly.Madeup.valueToCode(block, 'STOP', Blockly.Madeup.ORDER_NONE);
  var statements_body = Blockly.Madeup.statementToCode(block, 'BODY');
  var code = 'for ' + variable_iterator + ' in ' + value_start + '..' + value_stop + '\n' + statements_body + 'end\n';
  return code;
};

Blockly.Madeup['madeup_loop_for_in_by'] = function(block) {
  var value_start = Blockly.Madeup.valueToCode(block, 'START', Blockly.Madeup.ORDER_NONE);
  var variable_iterator = Blockly.Madeup.variableDB_.getName(block.getFieldValue('ITERATOR'), Blockly.Variables.NAME_TYPE);
  var value_next = Blockly.Madeup.valueToCode(block, 'NEXT', Blockly.Madeup.ORDER_NONE);
  var value_stop = Blockly.Madeup.valueToCode(block, 'STOP', Blockly.Madeup.ORDER_NONE);
  var statements_body = Blockly.Madeup.statementToCode(block, 'BODY');
  var code = 'for ' + variable_iterator + ' in ' + value_start + ',' + value_next + '..' + value_stop + '\n' + statements_body + 'end\n';
  return code;
};

Blockly.Madeup['variables_get'] = function(block) {
  var code = Blockly.Madeup.variableDB_.getName(block.getFieldValue('VAR'), Blockly.Variables.NAME_TYPE);
  return [code, Blockly.Madeup.ORDER_ATOMIC];
};

Blockly.Madeup['variables_set'] = function(block) {
  var argument0 = Blockly.Madeup.valueToCode(block, 'VALUE', Blockly.Madeup.ORDER_NONE) || '0';
  var varName = Blockly.Madeup.variableDB_.getName(block.getFieldValue('VAR'), Blockly.Variables.NAME_TYPE);
  return varName + ' = ' + argument0 + '\n';
};

Blockly.Madeup['procedures_defnoreturn'] = function(block) {
  var funcName = Blockly.Madeup.variableDB_.getName(block.getFieldValue('NAME'), Blockly.Procedures.NAME_TYPE);
  var branch = Blockly.Madeup.statementToCode(block, 'STACK');
  if (Blockly.Madeup.STATEMENT_PREFIX) {
    branch = Blockly.Madeup.prefixLines(Blockly.Madeup.STATEMENT_PREFIX.replace(/%1/g, '\'' + block.id + '\''), Blockly.Madeup.INDENT) + branch;
  }

  var args = [];
  for (var x = 0; x < block.arguments_.length; x++) {
    args[x] = Blockly.Madeup.variableDB_.getName(block.arguments_[x], Blockly.Variables.NAME_TYPE);
  }

  var code = 'to ' + funcName + ' ' + args.join(', ') + '\n' + branch + 'end';
  code = Blockly.Madeup.scrub_(block, code);
  Blockly.Madeup.definitions_[funcName] = code;
  return null;
};

Blockly.Madeup['procedures_callnoreturn'] = function(block) {
  var funcName = Blockly.Madeup.variableDB_.getName(block.getFieldValue('NAME'), Blockly.Procedures.NAME_TYPE);
  var args = [];
  for (var x = 0; x < block.arguments_.length; x++) {
    args[x] = Blockly.Madeup.valueToCode(block, 'ARG' + x, Blockly.Madeup.ORDER_ATOMIC) || '';
  }
  var code = funcName + ' ' + args.join(', ') + '\n';
  return code;
};

Blockly.Madeup['procedures_callreturn'] = function(block) {
  var funcName = Blockly.Madeup.variableDB_.getName(block.getFieldValue('NAME'), Blockly.Procedures.NAME_TYPE);
  var args = [];
  for (var x = 0; x < block.arguments_.length; x++) {
    args[x] = Blockly.Madeup.valueToCode(block, 'ARG' + x, Blockly.Madeup.ORDER_ATOMIC) || '';
  }
  var code = funcName + ' ' + args.join(', ');
  return [code, Blockly.Madeup.ORDER_ATOMIC];
};

Blockly.Madeup['madeup_string'] = function(block) {
  var text_string = block.getFieldValue('STRING');
  var code = '"' + text_string + '"';
  return [code, Blockly.Madeup.ORDER_ATOMIC];
};

Blockly.Madeup['madeup_eval'] = function(block) {
  var value_expr = Blockly.Madeup.valueToCode(block, 'EXPR', Blockly.Madeup.ORDER_NONE);
  var code = value_expr + '\n';
  return code;
};