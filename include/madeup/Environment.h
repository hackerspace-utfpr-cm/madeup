#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#include <chrono>
#include <map>
#include <string>
#include <vector>
#include <stack>

#include "madeup/GeometryMode.h"
#include "madeup/SourceLocation.h"
#include "twodee/Camera.h"
#include "twodee/Co.h"
#include "twodee/QMatrix4.h"
#include "twodee/QVector3.h"
#include "twodee/QVector4.h"
#include "twodee/Trimesh.h"

using std::map;
using std::pair;
using std::stack;
using std::string;
using std::vector;
using namespace td;

struct Node {
  QVector3<float> position;
  QVector3<float> rgb;
  float outer_radius;
  float inner_radius;
  float energy;
  float halflife;
};

struct Turtle {
  QVector3<float> position;
  Camera camera;
};

namespace madeup {

class ExpressionClosure;

/* ------------------------------------------------------------------------- */

class Environment {
  friend std::ostream &operator<<(std::ostream &out, const Environment env);
  public:
    Environment();
    Environment(const Environment &other);

    void add(const string &id, Co<ExpressionClosure> expression);
    void replace(const string &id, Co<ExpressionClosure> expression);
    bool isBound(const string &id) const;

    Co<ExpressionClosure> operator[](const string &id);

    void prime();
    void recordVertex();
    void recordPreview();
    void move(float distance);
    void movex(float distance);
    void moveTo(float x, float y, float z);
    void scale(float x, float y, float z);
    void translate(float x, float y, float z);
    void rotate(float x, float y, float z, float degrees);
    void center();
    void identity();
    int coalesce(float threshold);
    void home();
    void reverse();
    void echo(Co<Trimesh> mesh);
    void echo(const std::vector<Node> &path);
    void echoWithoutTransform(Co<Trimesh> mesh);
    void yaw(float degrees);
    void pitch(float degrees);
    void roll(float degrees);
    Co<Trimesh> dowel(float twist, float max_bend);
    Co<Trimesh> tube(float twist, float max_bend);
    Co<Trimesh> polygon(bool is_flipped);
    Co<Trimesh> revolve();
    Co<Trimesh> extrude(const QVector3<float> &axis, float length);
    Co<Trimesh> spheres();
    Co<Trimesh> boxes();
    Co<Trimesh> blobs(float grain, float iso);
    Co<Trimesh> surface(int width, int height);
    void forget();
    void axis(float x, float y, float z);
    void reframe();

    void push();
    void pop();

    void checkTimeout(const SourceLocation &location);

    Trimesh *getMesh();
    std::string getPathsJSON() const;
    void setGeometryMode(GeometryMode::geometry_mode_t mode);
    GeometryMode::geometry_mode_t getGeometryMode() const;

    float getVariableAsFloat(const std::string &id);
    const Turtle &getTurtle() const;
    void setTimeout(int max_seconds);
    std::vector<Node> popPath();

  private:
    bool hasMoved() const;

    map<string, Co<ExpressionClosure> > id_to_expression;
    std::chrono::high_resolution_clock::time_point start_time;
    int max_seconds;

    static Turtle turtle;
    static stack<Turtle> previous_turtles;
    static vector<Node> run;
    static Trimesh *shapes;
    static std::vector<std::vector<Turtle> > paths;
    static std::stack<QMatrix4<float> > xforms;
    static GeometryMode::geometry_mode_t geometry_mode;
};

/* ------------------------------------------------------------------------- */

std::ostream &operator<<(std::ostream &out, const Environment env);

/* ------------------------------------------------------------------------- */

}

#endif
