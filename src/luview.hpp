
#ifndef __LuviewObjects_HEADER__
#define __LuviewObjects_HEADER__


#include <vector>
#include "lua_object.hpp"

extern "C" {
#include "GL/glfw.h"
}


class CallbackFunction : public LuaCppObject
{
public:
  std::vector<double> call();
  std::vector<double> call(double u);
  std::vector<double> call(double u, double v);
  std::vector<double> call(double u, double v, double w);
  std::vector<double> call(std::vector<double> X);
  static CallbackFunction *CallbackFunction::create_from_stack(lua_State *L, int pos);
private:
  virtual std::vector<double> call_priv(double *x, int narg) = 0;
  int _call();
} ;

class LuaFunction : public CallbackFunction
{
private:
  virtual std::vector<double> call_priv(double *x, int narg);
} ;


class ColormapCollection : public CallbackFunction
{
public:
  ColormapCollection();
  virtual void next_colormap() = 0;
  virtual void prev_colormap() = 0;
protected:
  int cmap_id;
  int var_index;
protected:
  virtual LuaInstanceMethod __getattr__(std::string &method_name);
  static int _set_cmap_(lua_State *L);
  static int _set_component_(lua_State *L);
  static int _next_colormap_(lua_State *L);
  static int _prev_colormap_(lua_State *L);
} ;

class TessColormaps : public ColormapCollection
{
public:
  void next_colormap();
  void prev_colormap();
private:
  virtual std::vector<double> call_priv(double *x, int narg);
} ;

class MatplotlibColormaps : public ColormapCollection
{
public:
  void next_colormap();
  void prev_colormap();
private:
  virtual std::vector<double> call_priv(double *x, int narg);
} ;

class DataSource : public LuaCppObject
{
protected:
  DataSource *input;
  GLfloat *output;
  GLuint *indices;
  CallbackFunction *transform;

public:
  DataSource();
  ~DataSource();
  virtual GLfloat *get_data() { return output; }
  virtual GLuint *get_indices() { return indices; }
  virtual int get_num_points(int d) = 0;
  virtual int get_size() = 0;
  virtual int get_num_components() = 0;
  virtual int get_num_dimensions() = 0;

protected:
  virtual LuaInstanceMethod __getattr__(std::string &method_name);
  static int _get_transform_(lua_State *L);
  static int _set_transform_(lua_State *L);
  static int _get_input_(lua_State *L);
  static int _set_input_(lua_State *L);
} ;


class GlobalLinearTransformation : public DataSource
{
protected:
  // map component index to min value for normalization
  std::map<int, std::pair<double, double> > output_range;

public:
  virtual GLfloat *get_data();
  virtual int get_num_points(int d);
  virtual int get_size();
  virtual int get_num_components();
  virtual int get_num_dimensions();

protected:
  virtual LuaInstanceMethod __getattr__(std::string &method_name);
  static int _set_range_(lua_State *L);
} ;


class FunctionMapping : public DataSource
{
public:
  int get_num_points(int d);
  int get_size();
  int get_num_components();
  int get_num_dimensions();
  GLfloat *get_data();
} ;


class PointsSource :  public DataSource
{
protected:
  int Np, Nc;

public:
  PointsSource(lua_State *L, int pos);
  PointsSource();

  int get_num_points(int d);
  int get_size();
  int get_num_components();
  int get_num_dimensions();

protected:
  virtual LuaInstanceMethod __getattr__(std::string &method_name);
  static int _set_points_(lua_State *L);
} ;


class MultiImageSource :  public DataSource
{
protected:
  int Nx, Ny, Nc;

public:
  MultiImageSource();

  int get_num_points(int d);
  int get_size();
  int get_num_components();
  int get_num_dimensions();
  void set_array(double *data, int nx, int ny, int nc);

protected:
  virtual LuaInstanceMethod __getattr__(std::string &method_name);
  static int _set_array_(lua_State *L);
} ;


class Tesselation3D : public PointsSource
{
public:
  Tesselation3D();
  ~Tesselation3D();

  int get_num_points(int d);
  int get_size();
  int get_num_components();
  int get_num_dimensions();

  GLfloat *get_data();
protected:
  virtual LuaInstanceMethod __getattr__(std::string &method_name);
} ;




// ---------------------------------------------------------------------------
#define GETSET_TRAITS_D1(prop)                                          \
  static int _get_##prop##_(lua_State *L) {                             \
    LuviewTraitedObject *self = checkarg<LuviewTraitedObject>(L, 1);    \
    lua_remove(L, 1);                                                   \
    return __get_vec__(L, &self->prop, 1);                              \
  }                                                                     \
  static int _set_##prop##_(lua_State *L) {                             \
    LuviewTraitedObject *self = checkarg<LuviewTraitedObject>(L, 1);    \
    lua_remove(L, 1);                                                   \
    return __set_vec__(L, &self->prop, 1);                              \
  }                                                                     \
  // ---------------------------------------------------------------------------
#define GETSET_TRAITS_D3(prop)                                          \
  static int _get_##prop##_(lua_State *L) {                             \
    LuviewTraitedObject *self = checkarg<LuviewTraitedObject>(L, 1);    \
    lua_remove(L, 1);                                                   \
    return __get_vec__(L, self->prop, 3);                               \
  }                                                                     \
  static int _set_##prop##_(lua_State *L) {                             \
    LuviewTraitedObject *self = checkarg<LuviewTraitedObject>(L, 1);    \
    lua_remove(L, 1);                                                   \
    return __set_vec__(L, self->prop, 3);                               \
  }                                                                     \
  // ---------------------------------------------------------------------------


class LuviewTraitedObject : public LuaCppObject
{
protected:
  double Position[3];
  double Orientation[4];
  double Color[3];
  double Scale[3];
  double LineWidth;
  double Alpha;
  std::map<std::string, CallbackFunction*> Callbacks;
  std::map<std::string, DataSource*> DataSources;
  typedef std::map<std::string, CallbackFunction*>::iterator EntryCB;
  typedef std::map<std::string, DataSource*>::iterator EntryDS;

public:
  LuviewTraitedObject();

protected:
  virtual LuaInstanceMethod __getattr__(std::string &method_name);

  GETSET_TRAITS_D3(Position);
  GETSET_TRAITS_D3(Orientation);
  GETSET_TRAITS_D3(Color);
  GETSET_TRAITS_D3(Scale);
  GETSET_TRAITS_D1(LineWidth);
  GETSET_TRAITS_D1(Alpha);

  static int _get_Callback_(lua_State *L);
  static int _set_Callback_(lua_State *L);
  static int _get_DataSource_(lua_State *L);
  static int _set_DataSource_(lua_State *L);
  static int __get_vec__(lua_State *L, double *v, int n);
  static int __set_vec__(lua_State *L, double *v, int n);
} ;


class ShaderProgram : public LuaCppObject
{
private:
  GLuint vert, frag, prog;

public:
  ShaderProgram();
  ~ShaderProgram();

  void set_program(const char *vert_src, const char *frag_src);
  void unset_program();
  void activate();
  void deactivate();

private:
  void printShaderInfoLog(GLuint obj);
  void printProgramInfoLog(GLuint obj);

protected:
  virtual LuaInstanceMethod __getattr__(std::string &method_name);
  static int _set_program_(lua_State *L);
} ;


class DrawableObject : public LuviewTraitedObject
{
protected:
  std::vector<int> gl_modes;
  ShaderProgram *shader;

public:
  DrawableObject();
  virtual void draw();

protected:
  virtual void draw_local() = 0;

protected:
  virtual LuaInstanceMethod __getattr__(std::string &method_name);
  static int _get_shader_(lua_State *L);
  static int _set_shader_(lua_State *L);
} ;



class NbodySimulation : public DataSource
{
public:
  NbodySimulation();
  void advance();

  int get_num_points(int d) { return NumberOfParticles; }
  int get_size() { return NumberOfParticles; }
  int get_num_components() { return 3; }
  int get_num_dimensions() { return 1; }

private:
  int NumberOfParticles;
  double TimeStep;
  struct MassiveParticle
  {
    int id;
    double m;
    double x[3], v[3], a[3];
  } ;

  struct MassiveParticle *particles;
  void init_particles();

  void ComputeForces(struct MassiveParticle *P0, int N);
  void ComputeForces2(struct MassiveParticle *P0, int N);
  void MoveParticlesFwE(struct MassiveParticle *P0, int N, double dt);
  void MoveParticlesRK2(struct MassiveParticle *P0, int N, double dt);
  void MoveParticlesRK4(struct MassiveParticle *P0, int N, double dt);
  void (*MoveParticles)(struct MassiveParticle *P0, int N, double dt);

  double RandomDouble(double a, double b);

protected:
  virtual LuaInstanceMethod __getattr__(std::string &method_name);
  static int _advance_(lua_State *L);
} ;

#endif // __LuviewObjects_HEADER__
