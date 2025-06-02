/*
 *      Copyright (C) 2022 Jean-Luc Barriere
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "plugin_plplot.h"
#include "parseoptions.h"
#include "plplot/plstream.h"
#include <blocc/exception_runtime.h>
#include <blocc/collection.h>
#include <blocc/tuple.h>
#include <blocc/debug.h>
#include <vector>
#include <map>
/*
 * Create the module PLPLOTImport
 */
PLUGINCREATOR(PLPLOTPlugin)

namespace bloc
{
namespace plugin
{
namespace PLPLOT
{

/*
 * The module interface
 */

/**********************************************************************/
/*  Constructors                                                      */
/**********************************************************************/
static PLUGIN_TYPE ctor_0_args[]  = {
  { "L", 0 }, // device name
};

static PLUGIN_TYPE ctor_1_args[]  = {
  { "L", 0 }, // device name
  { "L", 0 }, // output filename
};


static PLUGIN_CTOR ctors[] =
{
  { 0,      1,   ctor_0_args,
          "Create new plot for the given device name.\n"
  "1  : device name" },
  { 1,      2,   ctor_1_args,
          "Create new plot for the given device name and output file name.\n"
  "1  : device name\n2  : output filename" },
};

enum Method
{
  Usage = 0, Version, Init, Init1, Flush, Replot, Pause, Pause1, Close,
  /* now ordered by alphanum */
  Adv, Axes, Bin, Box, Box3, Col0, Col1, Env, Env1, Errx, Erry, Fill, Fill3,
  Font, Lab, Lightsource, Line, Line3, Lsty, MinMax2dGrid, Mesh, Meshc, Mtex,
  Mtex3, Pat, Plot3d, Plot3dc, Poin, Poin3, Prec, Ptex, Ptex3, Schr, Scmap1n,
  Scmap1l, Scmap1l_1, Scmap1la, Scmap1la_1, Scol0a, Scolbga, String, String3,
  Surf3d, Surf3dl, Vpor, W3d, Width, Wind,
};

/**********************************************************************/
/*  Method arguments                                                  */
/*  mode:         type: decl,ndim                                     */
/**********************************************************************/
static PLUGIN_ARG init1_args[]  = {
  { PLUGIN_IN,    { "L", 0 } }, // options
};

static PLUGIN_ARG pause_args[]  = {
  { PLUGIN_IN,    { "B", 0 } }, // paused
};

static PLUGIN_ARG env_args[]  = {
  { PLUGIN_IN,    { "N", 0 } }, // xmin
  { PLUGIN_IN,    { "N", 0 } }, // xmax
  { PLUGIN_IN,    { "N", 0 } }, // ymin
  { PLUGIN_IN,    { "N", 0 } }, // ymax
};

static PLUGIN_ARG env1_args[]  = {
  { PLUGIN_IN,    { "N", 0 } }, // xmin
  { PLUGIN_IN,    { "N", 0 } }, // xmax
  { PLUGIN_IN,    { "N", 0 } }, // ymin
  { PLUGIN_IN,    { "N", 0 } }, // ymax
  { PLUGIN_IN,    { "I", 0 } }, // just
  { PLUGIN_IN,    { "I", 0 } }, // axis
};

static PLUGIN_ARG lab_args[]  = {
  { PLUGIN_IN,    { "L", 0 } }, // xlabel
  { PLUGIN_IN,    { "L", 0 } }, // ylabel
  { PLUGIN_IN,    { "L", 0 } }, // tlabel
};

static PLUGIN_ARG line_args[]  = {
  { PLUGIN_IN,    { "N", 1 } }, // x points
  { PLUGIN_IN,    { "N", 1 } }, // y points
};

static PLUGIN_ARG adv_args[]  = {
  { PLUGIN_IN,    { "I", 0 } }, // page
};

static PLUGIN_ARG vpor_args[]  = {
  { PLUGIN_IN,    { "N", 0 } }, // xmin
  { PLUGIN_IN,    { "N", 0 } }, // xmax
  { PLUGIN_IN,    { "N", 0 } }, // ymin
  { PLUGIN_IN,    { "N", 0 } }, // ymax
};

static PLUGIN_ARG wind_args[]  = {
  { PLUGIN_IN,    { "N", 0 } }, // xmin
  { PLUGIN_IN,    { "N", 0 } }, // xmax
  { PLUGIN_IN,    { "N", 0 } }, // ymin
  { PLUGIN_IN,    { "N", 0 } }, // ymax
};

static PLUGIN_ARG width_args[]  = {
  { PLUGIN_IN,    { "N", 0 } }, // width
};

static PLUGIN_ARG col0_args[]  = {
  { PLUGIN_IN,    { "I", 0 } }, // color
};

static PLUGIN_ARG col1_args[]  = {
  { PLUGIN_IN,    { "N", 0 } }, // col1
};

static PLUGIN_ARG scolbga_args[]  = {
  { PLUGIN_IN,    { "I", 0 } }, // r
  { PLUGIN_IN,    { "I", 0 } }, // g
  { PLUGIN_IN,    { "I", 0 } }, // b
  { PLUGIN_IN,    { "N", 0 } }, // alpha
};

static PLUGIN_ARG scol0a_args[]  = {
  { PLUGIN_IN,    { "I", 0 } }, // icol0
  { PLUGIN_IN,    { "I", 0 } }, // r
  { PLUGIN_IN,    { "I", 0 } }, // g
  { PLUGIN_IN,    { "I", 0 } }, // b
  { PLUGIN_IN,    { "N", 0 } }, // alpha
};

static PLUGIN_ARG ptex_args[]  = {
  { PLUGIN_IN,    { "N", 0 } }, // x coordinate
  { PLUGIN_IN,    { "N", 0 } }, // y coordinate
  { PLUGIN_IN,    { "N", 0 } }, // dx
  { PLUGIN_IN,    { "N", 0 } }, // dy
  { PLUGIN_IN,    { "N", 0 } }, // just
  { PLUGIN_IN,    { "L", 0 } }, // text
};

static PLUGIN_ARG mtex_args[]  = {
  { PLUGIN_IN,    { "L", 0 } }, // side
  { PLUGIN_IN,    { "N", 0 } }, // disp
  { PLUGIN_IN,    { "N", 0 } }, // pos
  { PLUGIN_IN,    { "N", 0 } }, // just
  { PLUGIN_IN,    { "L", 0 } }, // text
};

static PLUGIN_ARG box_args[]  = {
  { PLUGIN_IN,    { "L", 0 } }, // xopt
  { PLUGIN_IN,    { "N", 0 } }, // xtick
  { PLUGIN_IN,    { "I", 0 } }, // nxsub
  { PLUGIN_IN,    { "L", 0 } }, // yopt
  { PLUGIN_IN,    { "N", 0 } }, // ytick
  { PLUGIN_IN,    { "I", 0 } }, // nysub
};

static PLUGIN_ARG schr_args[]  = {
  { PLUGIN_IN,    { "N", 0 } }, // def
  { PLUGIN_IN,    { "N", 0 } }, // scale
};

static PLUGIN_ARG string_args[]  = {
  { PLUGIN_IN,    { "N", 1 } }, // x
  { PLUGIN_IN,    { "N", 1 } }, // y
  { PLUGIN_IN,    { "L", 0 } }, // string
};

static PLUGIN_ARG axes_args[]  = {
  { PLUGIN_IN,    { "N", 0 } }, // x0
  { PLUGIN_IN,    { "N", 0 } }, // y0
  { PLUGIN_IN,    { "L", 0 } }, // xopt
  { PLUGIN_IN,    { "N", 0 } }, // xtick
  { PLUGIN_IN,    { "I", 0 } }, // nxsub
  { PLUGIN_IN,    { "L", 0 } }, // yopt
  { PLUGIN_IN,    { "N", 0 } }, // ytick
  { PLUGIN_IN,    { "I", 0 } }, // nysub
};

static PLUGIN_ARG bin_args[]  = {
  { PLUGIN_IN,    { "N", 1 } }, // x
  { PLUGIN_IN,    { "N", 1 } }, // y
  { PLUGIN_IN,    { "I", 0 } }, // center
};

static PLUGIN_ARG errx_args[]  = {
  { PLUGIN_IN,    { "N", 1 } }, // xmin
  { PLUGIN_IN,    { "N", 1 } }, // xmax
  { PLUGIN_IN,    { "N", 1 } }, // y
};

static PLUGIN_ARG erry_args[]  = {
  { PLUGIN_IN,    { "N", 1 } }, // x
  { PLUGIN_IN,    { "N", 1 } }, // ymin
  { PLUGIN_IN,    { "N", 1 } }, // ymax
};

static PLUGIN_ARG fill_args[]  = {
  { PLUGIN_IN,    { "N", 1 } }, // x
  { PLUGIN_IN,    { "N", 1 } }, // y
};

static PLUGIN_ARG font_args[]  = {
  { PLUGIN_IN,    { "I", 0 } }, // font
};

static PLUGIN_ARG lsty_args[]  = {
  { PLUGIN_IN,    { "I", 0 } }, // lin
};

static PLUGIN_ARG minmax2dgrid_args[]  = {
  { PLUGIN_IN,    { "N", 2 } }, // z[x][y]
  { PLUGIN_IN,    { "N", 0 } }, // xpts
  { PLUGIN_IN,    { "N", 0 } }, // ypts
  { PLUGIN_INOUT, { "N", 0 } }, // zmax
  { PLUGIN_INOUT, { "N", 0 } }, // zmin
};

static PLUGIN_ARG w3d_args[]  = {
  { PLUGIN_IN,    { "N", 0 } }, // basex
  { PLUGIN_IN,    { "N", 0 } }, // basey
  { PLUGIN_IN,    { "N", 0 } }, // height
  { PLUGIN_IN,    { "N", 0 } }, // xmin
  { PLUGIN_IN,    { "N", 0 } }, // xmax
  { PLUGIN_IN,    { "N", 0 } }, // ymin
  { PLUGIN_IN,    { "N", 0 } }, // ymax
  { PLUGIN_IN,    { "N", 0 } }, // zmin
  { PLUGIN_IN,    { "N", 0 } }, // zmax
  { PLUGIN_IN,    { "N", 0 } }, // alt
  { PLUGIN_IN,    { "N", 0 } }, // az
};

static PLUGIN_ARG box3_args[]  = {
  { PLUGIN_IN,    { "L", 0 } }, // xopt
  { PLUGIN_IN,    { "L", 0 } }, // xlabel
  { PLUGIN_IN,    { "N", 0 } }, // xtick
  { PLUGIN_IN,    { "I", 0 } }, // nxsub
  { PLUGIN_IN,    { "L", 0 } }, // yopt
  { PLUGIN_IN,    { "L", 0 } }, // ylabel
  { PLUGIN_IN,    { "N", 0 } }, // ytick
  { PLUGIN_IN,    { "I", 0 } }, // nysub
  { PLUGIN_IN,    { "L", 0 } }, // zopt
  { PLUGIN_IN,    { "L", 0 } }, // zlabel
  { PLUGIN_IN,    { "N", 0 } }, // ztick
  { PLUGIN_IN,    { "I", 0 } }, // nzsub
};

static PLUGIN_ARG mesh_args[]  = {
  { PLUGIN_IN,    { "N", 1 } }, // x
  { PLUGIN_IN,    { "N", 1 } }, // y
  { PLUGIN_IN,    { "N", 2 } }, // z[x][y]
  { PLUGIN_IN,    { "I", 0 } }, // opt
};

static PLUGIN_ARG meshc_args[]  = {
  { PLUGIN_IN,    { "N", 1 } }, // x
  { PLUGIN_IN,    { "N", 1 } }, // y
  { PLUGIN_IN,    { "N", 2 } }, // z[x][y]
  { PLUGIN_IN,    { "I", 0 } }, // opt
  { PLUGIN_IN,    { "N", 1 } }, // clevel
};

static PLUGIN_ARG scmap1n_args[]  = {
  { PLUGIN_IN,    { "I", 0 } }, // ncol1
};

static PLUGIN_ARG scmap1l_args[]  = {
  { PLUGIN_IN,    { "B", 0 } }, // itype
  { PLUGIN_IN,    { "N", 1 } }, // intensity
  { PLUGIN_IN,    { "N", 1 } }, // coord1
  { PLUGIN_IN,    { "N", 1 } }, // coord2
  { PLUGIN_IN,    { "N", 1 } }, // coord3
  { PLUGIN_IN,    { "B", 1 } }, // alt_hue_path
};

static PLUGIN_ARG scmap1l_1_args[]  = {
  { PLUGIN_IN,    { "B", 0 } }, // itype
  { PLUGIN_IN,    { "N", 1 } }, // intensity
  { PLUGIN_IN,    { "N", 1 } }, // coord1
  { PLUGIN_IN,    { "N", 1 } }, // coord2
  { PLUGIN_IN,    { "N", 1 } }, // coord3
};

static PLUGIN_ARG scmap1la_args[]  = {
  { PLUGIN_IN,    { "B", 0 } }, // itype
  { PLUGIN_IN,    { "N", 1 } }, // intensity
  { PLUGIN_IN,    { "N", 1 } }, // coord1
  { PLUGIN_IN,    { "N", 1 } }, // coord2
  { PLUGIN_IN,    { "N", 1 } }, // coord3
  { PLUGIN_IN,    { "N", 1 } }, // alpha
  { PLUGIN_IN,    { "B", 1 } }, // alt_hue_path
};

static PLUGIN_ARG scmap1la_1_args[]  = {
  { PLUGIN_IN,    { "B", 0 } }, // itype
  { PLUGIN_IN,    { "N", 1 } }, // intensity
  { PLUGIN_IN,    { "N", 1 } }, // coord1
  { PLUGIN_IN,    { "N", 1 } }, // coord2
  { PLUGIN_IN,    { "N", 1 } }, // coord3
  { PLUGIN_IN,    { "N", 1 } }, // alpha
};

static PLUGIN_ARG string3_args[]  = {
  { PLUGIN_IN,    { "N", 1 } }, // x
  { PLUGIN_IN,    { "N", 1 } }, // y
  { PLUGIN_IN,    { "N", 1 } }, // z
  { PLUGIN_IN,    { "L", 0 } }, // string
};

static PLUGIN_ARG fill3_args[]  = {
  { PLUGIN_IN,    { "N", 1 } }, // x
  { PLUGIN_IN,    { "N", 1 } }, // y
  { PLUGIN_IN,    { "N", 1 } }, // z
};

static PLUGIN_ARG line3_args[]  = {
  { PLUGIN_IN,    { "N", 1 } }, // x
  { PLUGIN_IN,    { "N", 1 } }, // y
  { PLUGIN_IN,    { "N", 1 } }, // z
};

static PLUGIN_ARG lightsource_args[]  = {
  { PLUGIN_IN,    { "N", 0 } }, // x
  { PLUGIN_IN,    { "N", 0 } }, // y
  { PLUGIN_IN,    { "N", 0 } }, // z
};

static PLUGIN_ARG mtex3_args[]  = {
  { PLUGIN_IN,    { "L", 0 } }, // side
  { PLUGIN_IN,    { "N", 0 } }, // disp
  { PLUGIN_IN,    { "N", 0 } }, // pos
  { PLUGIN_IN,    { "N", 0 } }, // just
  { PLUGIN_IN,    { "L", 0 } }, // text
};

static PLUGIN_ARG poin_args[]  = {
  { PLUGIN_IN,    { "N", 1 } }, // x
  { PLUGIN_IN,    { "N", 1 } }, // y
  { PLUGIN_IN,    { "I", 0 } }, // code
};

static PLUGIN_ARG poin3_args[]  = {
  { PLUGIN_IN,    { "N", 1 } }, // x
  { PLUGIN_IN,    { "N", 1 } }, // y
  { PLUGIN_IN,    { "N", 1 } }, // z
  { PLUGIN_IN,    { "I", 0 } }, // code
};

static PLUGIN_ARG ptex3_args[]  = {
  { PLUGIN_IN,    { "N", 0 } }, // wx
  { PLUGIN_IN,    { "N", 0 } }, // wy
  { PLUGIN_IN,    { "N", 0 } }, // wz
  { PLUGIN_IN,    { "N", 0 } }, // dx
  { PLUGIN_IN,    { "N", 0 } }, // dy
  { PLUGIN_IN,    { "N", 0 } }, // dz
  { PLUGIN_IN,    { "N", 0 } }, // sx
  { PLUGIN_IN,    { "N", 0 } }, // sy
  { PLUGIN_IN,    { "N", 0 } }, // sz
  { PLUGIN_IN,    { "N", 0 } }, // just
  { PLUGIN_IN,    { "L", 0 } }, // text
};

static PLUGIN_ARG plot3d_args[]  = {
  { PLUGIN_IN,    { "N", 1 } }, // x
  { PLUGIN_IN,    { "N", 1 } }, // y
  { PLUGIN_IN,    { "N", 2 } }, // z[x][y]
  { PLUGIN_IN,    { "I", 0 } }, // opt
  { PLUGIN_IN,    { "B", 0 } }, // side
};

static PLUGIN_ARG plot3dc_args[]  = {
  { PLUGIN_IN,    { "N", 1 } }, // x
  { PLUGIN_IN,    { "N", 1 } }, // y
  { PLUGIN_IN,    { "N", 2 } }, // z[x][y]
  { PLUGIN_IN,    { "I", 0 } }, // opt
  { PLUGIN_IN,    { "N", 1 } }, // clevel
};

static PLUGIN_ARG prec_args[]  = {
  { PLUGIN_IN,    { "I", 0 } }, // setp
  { PLUGIN_IN,    { "I", 0 } }, // prec
};

static PLUGIN_ARG surf3d_args[]  = {
  { PLUGIN_IN,    { "N", 1 } }, // x
  { PLUGIN_IN,    { "N", 1 } }, // y
  { PLUGIN_IN,    { "N", 2 } }, // z[x][y]
  { PLUGIN_IN,    { "I", 0 } }, // opt
  { PLUGIN_IN,    { "N", 1 } }, // clevel
};

static PLUGIN_ARG surf3dl_args[]  = {
  { PLUGIN_IN,    { "N", 1 } }, // x
  { PLUGIN_IN,    { "N", 1 } }, // y
  { PLUGIN_IN,    { "N", 2 } }, // z[x][y]
  { PLUGIN_IN,    { "I", 0 } }, // opt
  { PLUGIN_IN,    { "N", 1 } }, // clevel
  { PLUGIN_IN,    { "I", 0 } }, // indexxmin
  { PLUGIN_IN,    { "I", 0 } }, // indexxmax
  { PLUGIN_IN,    { "I", 1 } }, // indexymin
  { PLUGIN_IN,    { "I", 1 } }, // indexymax
};

static PLUGIN_ARG pat_args[]  = {
  { PLUGIN_IN,    { "I", 1 } }, // inc
  { PLUGIN_IN,    { "I", 1 } }, // del
};

/**********************************************************************/
/*  Methods list                                                      */
/*  id:       name:         ret: decl,ndim  args_count,args:          */
/**********************************************************************/
static PLUGIN_METHOD methods[] =
{

  { Usage,    "usage",      { "B", 0 },     0, nullptr,
          "Print usage and syntax of options." },
  { Version,  "version",    { "L", 0 },     0, nullptr,
          "Get the current library version number." },
  { Init,     "init",       { "B", 0 },     0, nullptr,
          "Initialize the plotting session." },
  { Init1,    "init",       { "B", 0 },     1, init1_args,
          "Initialize the plotting session with the given options.\n"
  "1  : options" },
  { Flush,    "flush",      { "B", 0 },     0, nullptr,
          "Flushes the output stream." },
  { Replot,   "replot",     { "B", 0 },     0, nullptr,
          "Replays contents of plot buffer to current device/file." },
  { Pause,   "pause",       { "B", 0 },     1, pause_args,
          "Set the pause (on end-of-page) status. Default is off.\n"
  "1  : paused" },
  { Pause1,  "pause",       { "B", 0 },     0, nullptr,
          "Returns the pause status." },
  { Close,    "close",      { "B", 0 },     0, nullptr,
          "End plotting session." },
  { Adv,      "adv",        { "B", 0 },     1, adv_args,
          "Advance the (sub-)page.\n"
  "1  : page" },
  { Axes,     "axes",       { "B", 0 },     8, axes_args,
          "Draw a box with axes, etc. with arbitrary origin.\n"
  "1  : x0\n2  : y0\n3  : xopt\n4  : xtick\n5  : nxsub\n"
  "6  : yopt\n7  : ytick\n8  : nysub" },
  { Bin,      "bin",        { "B", 0 },     3, bin_args,
          "Plot a histogram from binned data.\n"
  "1  : x\n2  : y\n3  : center" },
  { Box,      "box",        { "B", 0 },     6, box_args,
          "Draw a box with axes, etc.\n"
  "1  : xopt\n2  : xtick\n3  : nxsub\n4  : yopt\n5  : ytick\n"
  "6  : nysub" },
  { Box3,     "box3",       { "B", 0 },     12, box3_args,
          "Draw a box with axes, etc, in 3D.\n"
  "1  : xopt\n2  : xlabel\n3  : xtick\n4  : nxsub\n5  : yopt\n"
  "6  : ylabel\n7  : ytick\n8  : nysub\n9  : zopt\n10 : zlabel\n"
  "11 : ztick\n12 : nzsub" },
  { Col0,     "col0",       { "B", 0 },     1, col0_args,
          "Sets the color index for cmap0.\n"
  "1  : color" },
  { Col1,     "col1",       { "B", 0 },     1, col1_args,
          "Sets the color index for cmap1.\n"
  "1  : col1" },
  { Env,      "env",        { "B", 0 },     4, env_args,
          "Set up standard window and draw box.\n"
  "1  : xmin\n2  : xmax\n3  : ymin\n4  : ymax" },
  { Env1,     "env",        { "B", 0 },     6, env1_args,
          "Set up standard window and draw box.\n"
  "1  : xmin\n2  : xmax\n3  : xymin\n4  : ymax\n5  : just\n"
  "6  : axis" },
  { Errx,     "errx",       { "B", 0 },     3, errx_args,
          "Draw error bars in the x direction.\n"
  "1  : xmin\n2  : xmax\n3  : y" },
  { Erry,     "erry",       { "B", 0 },     3, erry_args,
          "Draw error bars in the y direction.\n"
  "1  : x\n2  : ymin\n3  : ymax" },
  { Fill,     "fill",       { "B", 0 },     2, fill_args,
          "Draw filled polygon.\n"
  "1  : x\n2  : y" },
  { Fill3,    "fill3",      { "B", 0 },     3, fill3_args,
          "Draw filled polygon in 3D.\n"
  "1  : x\n2  : y\n3  : z" },
  { Font,     "font",       { "B", 0 },     1, font_args,
          "Set font.\n"
  "1  : font" },
  { Lab,      "lab",        { "B", 0 },     3, lab_args,
          "Simple routine to write labels.\n"
  "1  : xlabel\n2  : ylabel\n3  : tlabel" },
  { Lightsource,  "lightsource",  { "B", 0 }, 3, lightsource_args,
          "Sets the 3D position of the light source.\n"
  "1  : x\n2  : y\n3  : z" },
  { Line,     "line",       { "B", 0 },     2, line_args,
          "Draw a line.\n"
  "1  : x\n2  : y" },
  { Line3,    "line3",      { "B", 0 },     3, line3_args,
          "Draw a line in 3D.\n"
  "1  : x\n2  : y\n3  : z" },
  { Lsty,     "lsty",       { "B", 0 },     1, lsty_args,
          "Select line style.\n"
  "1  : lin" },
  { MinMax2dGrid, "minmax2dgrid", { "B", 0 }, 5, minmax2dgrid_args,
          "Find the minimum and maximum of a Z matrix.\n"
  "1  : f matrix\n2  : nx\n3  : ny\n4  : out zmax\n5  : out zmin" },
  { Mesh,     "mesh",       { "B", 0 },     4, mesh_args,
          "Plot surface mesh.\n"
  "1  : x\n2  : y\n3  : z\n4  : 1=DRAW_LINEX 2=DRAW_LINEY" },
  { Meshc,    "meshc",      { "B", 0 },     5, meshc_args,
          "Magnitude colored plot surface mesh with contour.\n"
  "1  : x\n2  : y\n3  : z\n"
  "4  : 1=DRAW_LINEX 2=DRAW_LINEY 4=MAG_COLOR 8=BASE_CONT 64=DRAW_SIDES\n"
  "5  : clevel" },
  { Mtex,      "mtex",      { "B", 0 },     5, mtex_args,
          "Write text relative to viewport boundaries.\n"
  "1  : side\n2  : disp\n3  : pos\n4  : just\n5  : text" },
  { Mtex3,    "mtex3",      { "B", 0 },     5, mtex3_args,
          "Write text relative to viewport boundaries in 3D plots.\n"
  "1  : side\n2  : disp\n3  : pos\n4  : just\n5  : text" },
  { Pat,      "pat",        { "B", 0 },     2, pat_args,
          "Set area line fill pattern.\n"
  "1  : inc\n2  : del" },
  { Poin,     "poin",       { "B", 0 },     3, poin_args,
          "Plot a glyph at the specified points.\n"
  "1  : x\n2  : y\n3  :code" },
  { Poin3,    "poin3",      { "B", 0 },     4, poin3_args,
          "Plot a glyph at the specified 3D points.\n"
  "1  : x\n2  : y\n3  : z\n4  : code" },
  { Plot3d,   "plot3d",     { "B", 0 },     5, plot3d_args,
          "Plot 3D surface plot.\n"
  "1  : x\n2  : y\n3  : z\n4  : 1=DRAW_LINEX 2=DRAW_LINEY\n"
  "5  : side" },
  { Plot3dc,  "plot3dc",    { "B", 0 },     5, plot3dc_args,
          "Magnitude colored plot surface with contour.\n"
  "1  : x\n2  : y\n3  : z\n"
  "4  : 1=DRAW_LINEX 2=DRAW_LINEY 4=MAG_COLOR 8=BASE_CONT 64=DRAW_SIDES\n"
  "5  : clevel" },
  { Prec,      "prec",      { "B", 0 },     2, prec_args,
          "Set precision in numeric labels.\n"
  "1  : setp\n2  : prec" },
  { Ptex,      "ptex",      { "B", 0 },     6, ptex_args,
          "Write text inside the viewport.\n"
  "1  : x\n2  : y\n3  : dx\n4  : dy\n5  : just\n6  : text" },
  { Ptex3,     "ptex3",     { "B", 0 },     11, ptex3_args,
          "Write text inside the viewport of a 3D plot.\n"
  "1  : wx\n2  : wy\n3  : wz\n4  : dx\n5  : dy\n6  : dz\n"
  "7  : sx\n8  : sy\n9  : sz\n10 : just\n11 : text" },
  { Schr,     "schr",       { "B", 0 },     2, schr_args,
          "Set character size.\n"
  "1  : def\n2  : scale" },
  { Scmap1n,  "scmap1n",    { "B", 0 },     1, scmap1n_args,
          "Set number of colors in cmap1.\n"
  "1  : ncol1" },
  { Scmap1l,  "scmap1l",    { "B", 0 },     6, scmap1l_args,
          "Set cmap1 colors using a piece-wise linear relationship.\n"
  "1  : itype\n2  : intensity\n3  : coord1\n4  : coord2\n5  : coord3\n"
  "6  : alt_hue_path" },
  { Scmap1l_1,  "scmap1l",  { "B", 0 },     5, scmap1l_1_args,
          "Set cmap1 colors using a piece-wise linear relationship.\n"
  "1  : itype\n2  : intensity\n3  : coord1\n4  : coord2\n5  : coord3" },
  { Scmap1la,   "scmap1la", { "B", 0 },     7, scmap1la_args,
          "Set cmap1 colors and alpha transparency using a piece-wise linear relationship.\n"
  "1  : itype\n2  : intensity\n3  : coord1\n4  : coord2\n5  : coord3\n6  : alpha\n"
  "7  : alt_hue_path" },
  { Scmap1la_1, "scmap1la", { "B", 0 },     6, scmap1la_1_args,
          "Set cmap1 colors and alpha transparency using a piece-wise linear relationship.\n"
  "1  : itype\n2  : intensity\n3  : coord1\n4  : coord2\n5  : coord3\n6  : alpha" },
  { Scol0a,   "scol0a",     { "B", 0 },     5, scol0a_args,
          "Set 8-bit RGB values and floating alpha transparency value for given cmap0 color index.\n"
  "1  : icol0\n2  : r\n3  : g\n4  : b\n5  : alpha" },
  { Scolbga,  "scolbga",    { "B", 0 },     4, scolbga_args,
          "Set the background color by 8-bit RGB value and floating alpha transparency value.\n"
  "1  : r\n2  : g\n3  : b\n4  : alpha" },
  { String,   "string",     { "B", 0 },     3, string_args,
          "Plot a glyph at the specified points.\n"
  "1  : x\n2  : y\n3  : string" },
  { String3,  "string3",    { "B", 0 },     4, string3_args,
          "Plot a glyph at the specified 3D points.\n"
  "1  : x\n2  : y\n3  : z\n4  : string" },
  { Surf3d,   "surf3d",     { "B", 0 },     5, surf3d_args,
          "Plot shaded 3D surface plot.\n"
  "1  : x\n2  : y\n3  : z\n"
  "4  : 4=MAG_COLOR 8=BASE_CONT 32=SURF_CONT 64=DRAW_SIDES 128=FACETED\n"
  "5  : clevel" },
  { Surf3dl,  "surf3dl",    { "B", 0 },     9, surf3dl_args,
          "Plot shaded 3D surface plot.\n"
  "1  : x\n2  : y\n3  : z\n"
  "4  : 4=MAG_COLOR 8=BASE_CONT 32=SURF_CONT 64=DRAW_SIDES 128=FACETED\n"
  "5  : clevel\n6  : indexxmin\n7  : indexxmax\n8  : indexymin\n9  : indexymax" },
  { Vpor,     "vpor",       { "B", 0 },     4, vpor_args,
          "Specify viewport using normalized subpage coordinates.\n"
  "1  : xmin\n2  : xmax\n3  : ymin\n4  : ymax" },
  { W3d,      "w3d",        { "B", 0 },     11, w3d_args,
          "Configure the transformations required for projecting a 3D surface on a 2D window.\n"
  "1  : basex\n2  : basey\n3  : height\n4  : xmin\n5  : xmax\n6  : ymin\n7  : ymax\n"
  "8  : zmin\n9  : zmax\n10 : alt\n11 : az" },
  { Width,    "width",      { "B", 0 },     1, width_args,
          "Set pen width.\n"
  "1  : width" },
  { Wind,     "wind",       { "B", 0 },     4, wind_args,
          "Specify window.\n"
  "1  : xmin\n2  : xmax\n3  : ymin\n4  : ymax" },
};

template<class T>
struct TabA
{
  T * data;
  size_t sn;
  explicit TabA(size_t n) : sn(0) { data = new T [n]; }
  ~TabA() { if (data != nullptr) delete [] data; }

  TabA(const TabA<T>&) = delete;
  TabA<T>& operator=(const TabA<T>&) = delete;
  TabA(TabA<T>&& t) : data(t.data), sn(t.sn)
  {
    t.data = nullptr;
    t.sn = 0;
  }
  TabA<T>& operator=(TabA<T>&& t)
  {
    this->data = t.data;
    this->sn = t.sn;
    t.data = nullptr;
    t.sn = 0;
    return *this;
  }

  T& at(size_t n) { return data[n]; }
};

TabA<double> col2taba(bloc::Collection& col, size_t n)
{
  TabA<double> tab(n);
  if (col.size() < n)
    n = col.size();
  for(size_t i = 0; i < n; ++i)
  {
    bloc::Value& v = col.at(i);
    if (v.isNull())
      throw RuntimeError(EXC_RT_OTHER_S, "Value cannot be null.");
    tab.at(i) = *v.numeric();
  }
  return std::move(tab);
}

TabA<bool> col2tabab(bloc::Collection& col, size_t n)
{
  TabA<bool> tab(n);
  if (col.size() < n)
    n = col.size();
  for(size_t i = 0; i < n; ++i)
  {
    bloc::Value& v = col.at(i);
    if (v.isNull())
      throw RuntimeError(EXC_RT_OTHER_S, "Value cannot be null.");
    tab.at(i) = *v.boolean();
  }
  return std::move(tab);
}

TabA<int> col2tabai(bloc::Collection& col, size_t n)
{
  TabA<int> tab(n);
  if (col.size() < n)
    n = col.size();
  for(size_t i = 0; i < n; ++i)
  {
    bloc::Value& v = col.at(i);
    if (v.isNull())
      throw RuntimeError(EXC_RT_OTHER_S, "Value cannot be null.");
    tab.at(i) = (int) *v.integer();
  }
  return std::move(tab);
}

template<class T>
struct TabZ
{
  T ** data;
  size_t sx;
  size_t sy;
  TabZ(size_t x, size_t y) : sx(x), sy(y)
  {
    data = new T*[sx];
    for (size_t i = 0; i < x; ++i)
      data[i] = new T[sy];
  }
  ~TabZ()
  {
    if (data != nullptr)
    {
      for (size_t i = 0; i < sx; ++i)
        delete [] data[i];
      delete [] data;
    }
  }

  TabZ(const TabZ<T>&) = delete;
  TabZ<T>& operator=(const TabZ<T>&) = delete;
  TabZ(TabZ<T>&& t) : data(t.data), sx(t.sx), sy(t.sy)
  {
    t.data = nullptr;
    t.sx = 0;
    t.sy = 0;
  }
  TabZ<T>& operator=(TabZ<T>&& t)
  {
    this->data = t.data;
    this->sx = t.sx;
    this->sy = t.sy;
    t.data = nullptr;
    t.sx = 0;
    t.sy = 0;
    return *this;
  }

  T& at(size_t x, size_t y) { return data[x][y]; }
};

TabZ<double> col2tabz(bloc::Collection& col, size_t x, size_t y)
{
  TabZ<double> tab(x, y);
  if (col.size() < x)
    x = col.size();
  for(size_t ix = 0; ix < x; ++ix)
  {
    if (col.at(ix).isNull())
      throw RuntimeError(EXC_RT_OTHER_S, "Value cannot be null.");
    bloc::Collection& c1 = *(col.at(ix).collection());
    size_t ny = y;
    if (c1.size() < ny)
      ny = c1.size();
    for(size_t iy = 0; iy < ny; ++iy)
    {
      bloc::Value& v = c1.at(iy);
      if (v.isNull())
        throw RuntimeError(EXC_RT_OTHER_S, "Value cannot be null.");
      tab.at(ix, iy) = (*v.numeric());
    }
  }
  return std::move(tab);
}

/**
 * The state of handle
 */
struct Handle {
  plstream *_pls;
  bool _paused;
  int _nx;
  int _ny;
  std::string _device;
  std::string _filename;
  std::string _msg;

  typedef struct { PLINT r; PLINT g; PLINT b; PLFLT a; } rgba_t;
  std::map<PLINT, rgba_t> _palette;

  ~Handle()
  {
    if (_pls)
      delete _pls;
  }
  Handle(const std::string& device, const std::string& filename) :
  _pls(nullptr),
  _paused(false),
  _nx(1),
  _ny(1),
  _device(device),
  _filename(filename) { reset(); }
  void reset();
  std::string version();
  bool init(const std::string& args);
  bool close();
  bool flush();
  bool replot();
  bool paused(bool enabled);
  bool env(double xmin, double xmax, double ymin, double ymax, int just, int axis);
  bool lab(const std::string& xlabel, const std::string& ylabel, const std::string& tlabel);
  bool line(int n, const double * x, const double * y);
  bool adv(int page);
  bool vpor(double xmin, double xmax, double ymin, double ymax);
  bool wind(double xmin, double xmax, double ymin, double ymax);
  bool width(double width);
  bool col0(int color);
  bool col1(double col1);
  bool scolbga(int r, int g, int b, double alpha);
  bool scol0a(int col0, int r, int g, int b, double alpha);
  bool ptex(double x, double y, double dx, double dy, double just, const std::string& text);
  bool mtex(const std::string& side, double disp, double pos, double just, const std::string& text);
  bool box(const std::string& xopt, double xtick, int nxsub, const std::string& yopt, double ytick, int nysub);
  bool schr(double def, double scale);
  bool string(int n, const double * x, const double * y, const std::string& str);
  bool axes(double x0, double y0, const std::string& xopt, double xtick, int nxsub, const std::string& yopt, double ytick, int nysub);
  bool bin(int nbin, const double * x, const double * y, int center);
  bool errx(int n, const double * xmin, const double * xmax, const double * y);
  bool erry(int n, const double * x, const double * ymin, const double * ymax);
  bool fill(int n, const double * x, const double * y);
  bool font(int ifont);
  bool lsty(int lin);
  bool poin(int n, const double * x, const double * y, int code);
  bool prec(int setp, int prec);
  bool pat(int nlin, const int * inc, const int * del);

  /* 3D */
  bool minmax2dgrid(const double * const * z, int nx, int ny, double * zmax, double * zmin );
  bool w3d(double basex, double basey, double height, double xmin, double xmax,
    double ymin, double ymax, double zmin, double zmax, double alt, double az);
  bool box3(const std::string& xopt, const std::string& xlabel, double xtick, int nxsub,
    const std::string& yopt, const std::string& ylabel, double ytick, int nysub,
    const std::string& zopt, const std::string& zlabel, double ztick, int nzsub);
  bool mesh(const double * x, const double * y, const double * const * z,
    int nx, int ny, int opt);
  bool meshc(const double * x, const double * y, const double * const * z,
    int nx, int ny, int opt, const double * clevel, int nlevel);
  bool scmap1n(int ncol1);
  bool scmap1l(bool itype, int npts, const double * intensity, const double * coord1, const double * coord2,
    const double * coord3, const bool * alt_hue_path = nullptr);
  bool scmap1la(bool itype, int npts, const double * intensity, const double * coord1, const double * coord2,
    const double * coord3, const double * alpha, const bool * alt_hue_path = nullptr);
  bool string3(int n, const double * x, const double * y, const double * z, const std::string& str);
  bool fill3(int n, const double * x, const double * y, const double * z);
  bool line3(int n, const double * x, const double * y, const double * z);
  bool lightsource(double x, double y, double z);
  bool mtex3(const std::string& side, double disp, double pos, double just, const std::string& text);
  bool poin3(int n, const double * x, const double * y, const double * z, int code);
  bool ptex3(double wx, double wy, double wz, double dx, double dy, double dz,
    double sx, double sy, double sz, double just, const std::string& text);
  bool plot3(const double * x, const double * y, const double * const * z, int nx, int ny, int opt, bool side);
  bool plot3c(const double * x, const double * y, const double * const * z, int nx, int ny, int opt, const double * clevel, int nlevel);
  bool surf3(const double * x, const double * y, const double * const * z, int nx, int ny, int opt, const double * clevel, int nlevel);
  bool surf3l(const double * x, const double * y, const double * const * z, int nx, int ny, int opt, const double * clevel, int nlevel,
    int indexxmin, int indexxmax, const int * indexymin, const int * indexymax);
};

} /* namespace PLPLOT */

void PLPLOTPlugin::declareInterface(PLUGIN_INTERFACE * interface)
{
  interface->name = "plplot";
  interface->method_count = sizeof(PLPLOT::methods) / sizeof(PLUGIN_METHOD);
  interface->methods = PLPLOT::methods;
  interface->ctors_count = sizeof(PLPLOT::ctors) / sizeof(PLUGIN_CTOR);
  interface->ctors = PLPLOT::ctors;
}

void * PLPLOTPlugin::createObject(int ctor_id, bloc::Context& ctx, const std::vector<bloc::Expression*>& args)
{
  PLPLOT::Handle * h = nullptr;
  switch(ctor_id)
  {
  case 0:
  {
    bloc::Value& a0 = args[0]->value(ctx);
    if (a0.isNull())
       throw RuntimeError(EXC_RT_OTHER_S, "Invalid arguments.");
    h = new PLPLOT::Handle(*a0.literal(), std::string());
    break;
  }
  case 1:
  {
    bloc::Value& a0 = args[0]->value(ctx);
    bloc::Value& a1 = args[1]->value(ctx);
    if (a0.isNull() || a1.isNull())
       throw RuntimeError(EXC_RT_OTHER_S, "Invalid arguments.");
    h = new PLPLOT::Handle(*a0.literal(), *a1.literal());
    break;
  }

  default: /* default ctor */
    h = new PLPLOT::Handle(std::string(), std::string());
    break;
  }
  return h;
}

void PLPLOTPlugin::destroyObject(void * object)
{
  PLPLOT::Handle * h = static_cast<PLPLOT::Handle*>(object);
  delete h;
}

bloc::Value * PLPLOTPlugin::executeMethod(
          bloc::Complex& object_this,
          int method_id,
          bloc::Context& ctx,
          const std::vector<bloc::Expression*>& args
          )
{
  PLPLOT::Handle * h = static_cast<PLPLOT::Handle*>(object_this.instance());

  switch (method_id)
  {
  case PLPLOT::Usage:
    h->_pls->OptUsage();
    return new bloc::Value(bloc::Bool(true));

  case PLPLOT::Version:
    return new bloc::Value(new bloc::Literal(h->version()));

  case PLPLOT::Init:
    return new bloc::Value(bloc::Bool(h->init(std::string())));

  case PLPLOT::Init1:
  {
    bloc::Value& a0 = args[0]->value(ctx);
    if (a0.isNull())
      throw RuntimeError(EXC_RT_OTHER_S, "Invalid arguments.");
    return new bloc::Value(bloc::Bool(h->init(*a0.literal())));
  }

  case PLPLOT::Close:
    return new bloc::Value(bloc::Bool(h->close()));

  case PLPLOT::Flush:
    return new bloc::Value(bloc::Bool(h->flush()));

  case PLPLOT::Replot:
    return new bloc::Value(bloc::Bool(h->replot()));

  case PLPLOT::Pause:
  {
    bloc::Value& a0 = args[0]->value(ctx);
    if (a0.isNull())
      throw RuntimeError(EXC_RT_OTHER_S, "Invalid arguments.");
    return new bloc::Value(bloc::Bool(h->paused(*a0.boolean())));
  }

  case PLPLOT::Pause1:
  {
    return new bloc::Value(bloc::Bool(h->_paused));
  }

  case PLPLOT::Env:
  {
    bloc::Value& a0 = args[0]->value(ctx);
    bloc::Value& a1 = args[1]->value(ctx);
    bloc::Value& a2 = args[2]->value(ctx);
    bloc::Value& a3 = args[3]->value(ctx);
    if (a0.isNull() || a1.isNull() || a2.isNull() || a3.isNull())
      throw RuntimeError(EXC_RT_OTHER_S, "Invalid arguments.");
    double xmin = (a0.type() == bloc::Type::NUMERIC ? *a0.numeric() : *a0.integer());
    double xmax = (a1.type() == bloc::Type::NUMERIC ? *a1.numeric() : *a1.integer());
    double ymin = (a2.type() == bloc::Type::NUMERIC ? *a2.numeric() : *a2.integer());
    double ymax = (a3.type() == bloc::Type::NUMERIC ? *a3.numeric() : *a3.integer());
    return new bloc::Value(bloc::Bool(h->env(xmin, xmax, ymin, ymax, 0, 0)));
  }

  case PLPLOT::Env1:
  {
    bloc::Value& a0 = args[0]->value(ctx);
    bloc::Value& a1 = args[1]->value(ctx);
    bloc::Value& a2 = args[2]->value(ctx);
    bloc::Value& a3 = args[3]->value(ctx);
    bloc::Value& a4 = args[4]->value(ctx);
    bloc::Value& a5 = args[5]->value(ctx);
    if (a0.isNull() || a1.isNull() || a2.isNull() || a3.isNull() || a4.isNull() || a5.isNull())
      throw RuntimeError(EXC_RT_OTHER_S, "Invalid arguments.");
    double xmin = (a0.type() == bloc::Type::NUMERIC ? *a0.numeric() : *a0.integer());
    double xmax = (a1.type() == bloc::Type::NUMERIC ? *a1.numeric() : *a1.integer());
    double ymin = (a2.type() == bloc::Type::NUMERIC ? *a2.numeric() : *a2.integer());
    double ymax = (a3.type() == bloc::Type::NUMERIC ? *a3.numeric() : *a3.integer());
    int just = (int) *a4.integer();
    int axis = (int) *a5.integer();
    return new bloc::Value(bloc::Bool(h->env(xmin, xmax, ymin, ymax, just, axis)));
  }

  case PLPLOT::Lab:
  {
    bloc::Value& a0 = args[0]->value(ctx);
    bloc::Value& a1 = args[1]->value(ctx);
    bloc::Value& a2 = args[2]->value(ctx);
    if (a0.isNull() || a1.isNull() || a2.isNull())
      throw RuntimeError(EXC_RT_OTHER_S, "Invalid arguments.");
    return new bloc::Value(bloc::Bool(h->lab(*a0.literal(), *a1.literal(), *a2.literal())));
  }

  case PLPLOT::Line:
  {
    bloc::Value& a0 = args[0]->value(ctx);
    bloc::Value& a1 = args[1]->value(ctx);
    if (a0.isNull() || a1.isNull())
      throw RuntimeError(EXC_RT_OTHER_S, "Invalid arguments.");
    bloc::Collection& x = *a0.collection();
    bloc::Collection& y = *a1.collection();
    size_t n = (x.size() > y.size() ? y.size() : x.size());
    PLPLOT::TabA<double> vx = PLPLOT::col2taba(x, n);
    PLPLOT::TabA<double> vy = PLPLOT::col2taba(y, n);
    return new bloc::Value(bloc::Bool(h->line((int) n, vx.data, vy.data)));
  }

  case PLPLOT::Adv:
  {
    bloc::Value& a0 = args[0]->value(ctx);
    if (a0.isNull())
      throw RuntimeError(EXC_RT_OTHER_S, "Invalid arguments.");
    return new bloc::Value(bloc::Bool(h->adv((int) *a0.integer())));
  }

  case PLPLOT::Vpor:
  {
    bloc::Value& a0 = args[0]->value(ctx);
    bloc::Value& a1 = args[1]->value(ctx);
    bloc::Value& a2 = args[2]->value(ctx);
    bloc::Value& a3 = args[3]->value(ctx);
    if (a0.isNull() || a1.isNull() || a2.isNull() || a3.isNull())
      throw RuntimeError(EXC_RT_OTHER_S, "Invalid arguments.");
    double xmin = (a0.type() == bloc::Type::NUMERIC ? *a0.numeric() : *a0.integer());
    double xmax = (a1.type() == bloc::Type::NUMERIC ? *a1.numeric() : *a1.integer());
    double ymin = (a2.type() == bloc::Type::NUMERIC ? *a2.numeric() : *a2.integer());
    double ymax = (a3.type() == bloc::Type::NUMERIC ? *a3.numeric() : *a3.integer());
    return new bloc::Value(bloc::Bool(h->vpor(xmin, xmax, ymin, ymax)));
  }

  case PLPLOT::Wind:
  {
    bloc::Value& a0 = args[0]->value(ctx);
    bloc::Value& a1 = args[1]->value(ctx);
    bloc::Value& a2 = args[2]->value(ctx);
    bloc::Value& a3 = args[3]->value(ctx);
    if (a0.isNull() || a1.isNull() || a2.isNull() || a3.isNull())
      throw RuntimeError(EXC_RT_OTHER_S, "Invalid arguments.");
    double xmin = (a0.type() == bloc::Type::NUMERIC ? *a0.numeric() : *a0.integer());
    double xmax = (a1.type() == bloc::Type::NUMERIC ? *a1.numeric() : *a1.integer());
    double ymin = (a2.type() == bloc::Type::NUMERIC ? *a2.numeric() : *a2.integer());
    double ymax = (a3.type() == bloc::Type::NUMERIC ? *a3.numeric() : *a3.integer());
    return new bloc::Value(bloc::Bool(h->wind(xmin, xmax, ymin, ymax)));
  }

  case PLPLOT::Width:
  {
    bloc::Value& a0 = args[0]->value(ctx);
    if (a0.isNull())
      throw RuntimeError(EXC_RT_OTHER_S, "Invalid arguments.");
    double w = (a0.type() == bloc::Type::NUMERIC ? *a0.numeric() : *a0.integer());
    return new bloc::Value(bloc::Bool(h->width(w)));
  }

  case PLPLOT::Col0:
  {
    bloc::Value& a0 = args[0]->value(ctx);
    if (a0.isNull())
      throw RuntimeError(EXC_RT_OTHER_S, "Invalid arguments.");
    return new bloc::Value(bloc::Bool(h->col0(*a0.integer())));
  }

  case PLPLOT::Col1:
  {
    bloc::Value& a0 = args[0]->value(ctx);
    if (a0.isNull())
      throw RuntimeError(EXC_RT_OTHER_S, "Invalid arguments.");
    return new bloc::Value(bloc::Bool(h->col1(*a0.numeric())));
  }

  case PLPLOT::Scolbga:
  {
    bloc::Value& a0 = args[0]->value(ctx);
    bloc::Value& a1 = args[1]->value(ctx);
    bloc::Value& a2 = args[2]->value(ctx);
    bloc::Value& a3 = args[3]->value(ctx);
    if (a0.isNull() || a1.isNull() || a2.isNull() || a3.isNull())
      throw RuntimeError(EXC_RT_OTHER_S, "Invalid arguments.");
    return new bloc::Value(bloc::Bool(h->scolbga((int) *a0.integer(), (int) *a1.integer(), (int) *a2.integer(), *a3.numeric())));
  }

  case PLPLOT::Scol0a:
  {
    bloc::Value& a0 = args[0]->value(ctx);
    bloc::Value& a1 = args[1]->value(ctx);
    bloc::Value& a2 = args[2]->value(ctx);
    bloc::Value& a3 = args[3]->value(ctx);
    bloc::Value& a4 = args[4]->value(ctx);
    if (a0.isNull() || a1.isNull() || a2.isNull() || a3.isNull() || a4.isNull())
      throw RuntimeError(EXC_RT_OTHER_S, "Invalid arguments.");
    return new bloc::Value(bloc::Bool(h->scol0a((int) *a0.integer(), (int) *a1.integer(), (int) *a2.integer(), (int) *a3.integer(), *a4.numeric())));
  }

  case PLPLOT::Ptex:
  {
    bloc::Value& a0 = args[0]->value(ctx);
    bloc::Value& a1 = args[1]->value(ctx);
    bloc::Value& a2 = args[2]->value(ctx);
    bloc::Value& a3 = args[3]->value(ctx);
    bloc::Value& a4 = args[4]->value(ctx);
    bloc::Value& a5 = args[5]->value(ctx);
    if (a0.isNull() || a1.isNull() || a2.isNull() || a3.isNull() || a4.isNull()
       || a5.isNull())
      throw RuntimeError(EXC_RT_OTHER_S, "Invalid arguments.");
    double x = (a0.type() == bloc::Type::NUMERIC ? *a0.numeric() : *a0.integer());
    double y = (a1.type() == bloc::Type::NUMERIC ? *a1.numeric() : *a1.integer());
    double dx = (a2.type() == bloc::Type::NUMERIC ? *a2.numeric() : *a2.integer());
    double dy = (a3.type() == bloc::Type::NUMERIC ? *a3.numeric() : *a3.integer());
    double just = (a4.type() == bloc::Type::NUMERIC ? *a4.numeric() : *a4.integer());
    return new bloc::Value(bloc::Bool(h->ptex(x, y, dx, dy, just, *a5.literal())));
  }

  case PLPLOT::Mtex:
  {
    bloc::Value& a0 = args[0]->value(ctx);
    bloc::Value& a1 = args[1]->value(ctx);
    bloc::Value& a2 = args[2]->value(ctx);
    bloc::Value& a3 = args[3]->value(ctx);
    bloc::Value& a4 = args[4]->value(ctx);
    if (a0.isNull() || a1.isNull() || a2.isNull() || a3.isNull() || a4.isNull())
      throw RuntimeError(EXC_RT_OTHER_S, "Invalid arguments.");
    double disp = (a1.type() == bloc::Type::NUMERIC ? *a1.numeric() : *a1.integer());
    double pos = (a2.type() == bloc::Type::NUMERIC ? *a2.numeric() : *a2.integer());
    double just = (a3.type() == bloc::Type::NUMERIC ? *a3.numeric() : *a3.integer());
    return new bloc::Value(bloc::Bool(h->mtex(*a0.literal(), disp, pos, just, *a4.literal())));
  }

  case PLPLOT::Box:
  {
    bloc::Value& a0 = args[0]->value(ctx);
    bloc::Value& a1 = args[1]->value(ctx);
    bloc::Value& a2 = args[2]->value(ctx);
    bloc::Value& a3 = args[3]->value(ctx);
    bloc::Value& a4 = args[4]->value(ctx);
    bloc::Value& a5 = args[5]->value(ctx);
    if (a0.isNull() || a1.isNull() || a2.isNull() || a3.isNull() || a4.isNull()
            || a5.isNull())
      throw RuntimeError(EXC_RT_OTHER_S, "Invalid arguments.");
    double xtick = (a1.type() == bloc::Type::NUMERIC ? *a1.numeric() : *a1.integer());
    double ytick = (a4.type() == bloc::Type::NUMERIC ? *a4.numeric() : *a4.integer());
    return new bloc::Value(bloc::Bool(h->box(*a0.literal(), xtick, (int) *a2.integer(), *a3.literal(), ytick, (int) *a5.integer())));
  }

  case PLPLOT::Schr:
  {
    bloc::Value& a0 = args[0]->value(ctx);
    bloc::Value& a1 = args[1]->value(ctx);
    if (a0.isNull() || a1.isNull())
      throw RuntimeError(EXC_RT_OTHER_S, "Invalid arguments.");
    double def = (a0.type() == bloc::Type::NUMERIC ? *a0.numeric() : *a0.integer());
    double scale = (a1.type() == bloc::Type::NUMERIC ? *a1.numeric() : *a1.integer());
    return new bloc::Value(bloc::Bool(h->schr(def, scale)));
  }

  case PLPLOT::String:
  {
    bloc::Value& a0 = args[0]->value(ctx);
    bloc::Value& a1 = args[1]->value(ctx);
    bloc::Value& a2 = args[2]->value(ctx);
    if (a0.isNull() || a1.isNull() || a2.isNull())
      throw RuntimeError(EXC_RT_OTHER_S, "Invalid arguments.");
    bloc::Collection& x = *a0.collection();
    bloc::Collection& y = *a1.collection();
    size_t n = (x.size() > y.size() ? y.size() : x.size());
    PLPLOT::TabA<double> vx = PLPLOT::col2taba(x, n);
    PLPLOT::TabA<double> vy = PLPLOT::col2taba(y, n);
    return new bloc::Value(bloc::Bool(h->string((int) n, vx.data, vy.data, *a2.literal())));
  }

  case PLPLOT::Axes:
  {
    bloc::Value& a0 = args[0]->value(ctx);
    bloc::Value& a1 = args[1]->value(ctx);
    bloc::Value& a2 = args[2]->value(ctx);
    bloc::Value& a3 = args[3]->value(ctx);
    bloc::Value& a4 = args[4]->value(ctx);
    bloc::Value& a5 = args[5]->value(ctx);
    bloc::Value& a6 = args[6]->value(ctx);
    bloc::Value& a7 = args[7]->value(ctx);
    if (a0.isNull() || a1.isNull() || a2.isNull() || a3.isNull() || a4.isNull()
             || a5.isNull() || a6.isNull() || a7.isNull())
      throw RuntimeError(EXC_RT_OTHER_S, "Invalid arguments.");
    double x0 = (a0.type() == bloc::Type::NUMERIC ? *a0.numeric() : *a0.integer());
    double y0 = (a1.type() == bloc::Type::NUMERIC ? *a1.numeric() : *a1.integer());
    double xt = (a3.type() == bloc::Type::NUMERIC ? *a3.numeric() : *a3.integer());
    double yt = (a6.type() == bloc::Type::NUMERIC ? *a6.numeric() : *a6.integer());
    return new bloc::Value(bloc::Bool(h->axes(x0, y0, *a2.literal(), xt, (int) *a4.integer(),
            *a5.literal(), yt, (int) *a7.integer())));
  }

  case PLPLOT::Bin:
  {
    bloc::Value& a0 = args[0]->value(ctx);
    bloc::Value& a1 = args[1]->value(ctx);
    bloc::Value& a2 = args[2]->value(ctx);
    if (a0.isNull() || a1.isNull() || a2.isNull())
      throw RuntimeError(EXC_RT_OTHER_S, "Invalid arguments.");
    bloc::Collection& x = *a0.collection();
    bloc::Collection& y = *a1.collection();
    size_t n = (x.size() > y.size() ? y.size() : x.size());
    PLPLOT::TabA<double> vx = PLPLOT::col2taba(x, n);
    PLPLOT::TabA<double> vy = PLPLOT::col2taba(y, n);
    return new bloc::Value(bloc::Bool(h->bin((int) n, vx.data, vy.data, (int) *a2.integer())));
  }

  case PLPLOT::Errx:
  {
    bloc::Value& a0 = args[0]->value(ctx);
    bloc::Value& a1 = args[1]->value(ctx);
    bloc::Value& a2 = args[2]->value(ctx);
    if (a0.isNull() || a1.isNull() || a2.isNull())
      throw RuntimeError(EXC_RT_OTHER_S, "Invalid arguments.");
    bloc::Collection& xmin = *a0.collection();
    bloc::Collection& xmax = *a1.collection();
    bloc::Collection& y = *a2.collection();
    size_t n = (xmin.size() > xmax.size() ? xmax.size() : xmin.size());
    if (y.size() > n)
      n = y.size();
    PLPLOT::TabA<double> vmin = PLPLOT::col2taba(xmin, n);
    PLPLOT::TabA<double> vmax = PLPLOT::col2taba(xmax, n);
    PLPLOT::TabA<double> vy = PLPLOT::col2taba(y, n);
    return new bloc::Value(bloc::Bool(h->errx((int) n, vmin.data, vmax.data, vy.data)));
  }

  case PLPLOT::Erry:
  {
    bloc::Value& a0 = args[0]->value(ctx);
    bloc::Value& a1 = args[1]->value(ctx);
    bloc::Value& a2 = args[2]->value(ctx);
    if (a0.isNull() || a1.isNull() || a2.isNull())
      throw RuntimeError(EXC_RT_OTHER_S, "Invalid arguments.");
    bloc::Collection& x = *a0.collection();
    bloc::Collection& ymin = *a1.collection();
    bloc::Collection& ymax = *a2.collection();
    size_t n = (x.size() > ymin.size() ? ymin.size() : x.size());
    if (ymax.size() > n)
      n = ymax.size();
    PLPLOT::TabA<double> vx = PLPLOT::col2taba(x, n);
    PLPLOT::TabA<double> vmin = PLPLOT::col2taba(ymin, n);
    PLPLOT::TabA<double> vmax = PLPLOT::col2taba(ymax, n);
    return new bloc::Value(bloc::Bool(h->errx((int) n, vx.data, vmin.data, vmax.data)));
  }

  case PLPLOT::Fill:
  {
    bloc::Value& a0 = args[0]->value(ctx);
    bloc::Value& a1 = args[1]->value(ctx);
    if (a0.isNull() || a1.isNull())
      throw RuntimeError(EXC_RT_OTHER_S, "Invalid arguments.");
    bloc::Collection& x = *a0.collection();
    bloc::Collection& y = *a1.collection();
    size_t n = (x.size() > y.size() ? y.size() : x.size());
    PLPLOT::TabA<double> vx = PLPLOT::col2taba(x, n);
    PLPLOT::TabA<double> vy = PLPLOT::col2taba(y, n);
    return new bloc::Value(bloc::Bool(h->fill((int) n, vx.data, vy.data)));
  }

  case PLPLOT::Font:
  {
    bloc::Value& a0 = args[0]->value(ctx);
    if (a0.isNull())
      throw RuntimeError(EXC_RT_OTHER_S, "Invalid arguments.");
    return new bloc::Value(bloc::Bool(h->font((int) *a0.integer())));
  }

  case PLPLOT::Lsty:
  {
    bloc::Value& a0 = args[0]->value(ctx);
    if (a0.isNull())
      throw RuntimeError(EXC_RT_OTHER_S, "Invalid arguments.");
    return new bloc::Value(bloc::Bool(h->lsty((int) *a0.integer())));
  }

  case PLPLOT::MinMax2dGrid:
  {
    bloc::Value& a0 = args[0]->value(ctx);
    bloc::Value& a1 = args[1]->value(ctx);
    bloc::Value& a2 = args[2]->value(ctx);
    if (a0.isNull() || a1.isNull() || a2.isNull()
            || args[3]->symbol() == nullptr || args[4]->symbol() == nullptr)
      throw RuntimeError(EXC_RT_OTHER_S, "Invalid arguments.");

    bloc::Collection& z = *a0.collection();
    size_t nx = *a1.integer();
    size_t ny = *a2.integer();
    PLPLOT::TabZ<double> vz = PLPLOT::col2tabz(z, nx, ny);
    double zmax;
    double zmin;
    if (h->minmax2dgrid(vz.data, nx, ny, &zmax, &zmin))
    {
      /* INOUT */
      ctx.storeVariable(*args[3]->symbol(), bloc::Value(bloc::Numeric(zmax)));
      ctx.storeVariable(*args[4]->symbol(), bloc::Value(bloc::Numeric(zmin)));
      return new bloc::Value(bloc::Bool(true));
    }
    return new bloc::Value(bloc::Bool(false));
  }

  case PLPLOT::W3d:
  {
    bloc::Value& a0 = args[0]->value(ctx);
    bloc::Value& a1 = args[1]->value(ctx);
    bloc::Value& a2 = args[2]->value(ctx);
    bloc::Value& a3 = args[3]->value(ctx);
    bloc::Value& a4 = args[4]->value(ctx);
    bloc::Value& a5 = args[5]->value(ctx);
    bloc::Value& a6 = args[6]->value(ctx);
    bloc::Value& a7 = args[7]->value(ctx);
    bloc::Value& a8 = args[8]->value(ctx);
    bloc::Value& a9 = args[9]->value(ctx);
    bloc::Value& a10 = args[10]->value(ctx);
    if (a0.isNull() || a1.isNull() || a2.isNull() || a3.isNull() || a4.isNull()
             || a5.isNull() || a6.isNull() || a7.isNull() || a8.isNull()
             || a9.isNull() || a10.isNull())
      throw RuntimeError(EXC_RT_OTHER_S, "Invalid arguments.");
    double basex = (a0.type() == bloc::Type::NUMERIC ? *a0.numeric() : *a0.integer());
    double basey = (a1.type() == bloc::Type::NUMERIC ? *a1.numeric() : *a1.integer());
    double height = (a2.type() == bloc::Type::NUMERIC ? *a2.numeric() : *a2.integer());
    double xmin = (a3.type() == bloc::Type::NUMERIC ? *a3.numeric() : *a3.integer());
    double xmax = (a4.type() == bloc::Type::NUMERIC ? *a4.numeric() : *a4.integer());
    double ymin = (a5.type() == bloc::Type::NUMERIC ? *a5.numeric() : *a5.integer());
    double ymax = (a6.type() == bloc::Type::NUMERIC ? *a6.numeric() : *a6.integer());
    double zmin = (a7.type() == bloc::Type::NUMERIC ? *a7.numeric() : *a7.integer());
    double zmax = (a8.type() == bloc::Type::NUMERIC ? *a8.numeric() : *a8.integer());
    double alt = (a9.type() == bloc::Type::NUMERIC ? *a9.numeric() : *a9.integer());
    double az = (a10.type() == bloc::Type::NUMERIC ? *a10.numeric() : *a10.integer());
    return new bloc::Value(bloc::Bool(h->w3d(basex, basey, height, xmin, xmax,
                                             ymin, ymax, zmin, zmax, alt, az)));
  }

  case PLPLOT::Box3:
  {
    bloc::Value& a0 = args[0]->value(ctx);
    bloc::Value& a1 = args[1]->value(ctx);
    bloc::Value& a2 = args[2]->value(ctx);
    bloc::Value& a3 = args[3]->value(ctx);
    bloc::Value& a4 = args[4]->value(ctx);
    bloc::Value& a5 = args[5]->value(ctx);
    bloc::Value& a6 = args[6]->value(ctx);
    bloc::Value& a7 = args[7]->value(ctx);
    bloc::Value& a8 = args[8]->value(ctx);
    bloc::Value& a9 = args[9]->value(ctx);
    bloc::Value& a10 = args[10]->value(ctx);
    bloc::Value& a11 = args[11]->value(ctx);
    if (a0.isNull() || a1.isNull() || a2.isNull() || a3.isNull() || a4.isNull()
            || a5.isNull() || a6.isNull() || a7.isNull() || a8.isNull()
            || a9.isNull() || a10.isNull() || a11.isNull())
      throw RuntimeError(EXC_RT_OTHER_S, "Invalid arguments.");
    double xtick = (a2.type() == bloc::Type::NUMERIC ? *a2.numeric() : *a2.integer());
    double ytick = (a6.type() == bloc::Type::NUMERIC ? *a6.numeric() : *a6.integer());
    double ztick = (a10.type() == bloc::Type::NUMERIC ? *a10.numeric() : *a10.integer());
    return new bloc::Value(bloc::Bool(h->box3(
            *a0.literal(), *a1.literal(), xtick, (int) *a3.integer(),
            *a4.literal(), *a5.literal(), ytick, (int) *a7.integer(),
            *a8.literal(), *a9.literal(), ztick, (int) *a11.integer())));
  }

  case PLPLOT::Mesh:
  {
    bloc::Value& a0 = args[0]->value(ctx);
    bloc::Value& a1 = args[1]->value(ctx);
    bloc::Value& a2 = args[2]->value(ctx);
    bloc::Value& a3 = args[3]->value(ctx);
    if (a0.isNull() || a1.isNull() || a2.isNull() || a3.isNull())
      throw RuntimeError(EXC_RT_OTHER_S, "Invalid arguments.");
    bloc::Collection& x = *a0.collection();
    bloc::Collection& y = *a1.collection();
    bloc::Collection& z = *a2.collection();
    size_t nx = x.size();
    size_t ny = y.size();
    PLPLOT::TabZ<double> vz = PLPLOT::col2tabz(z, nx, ny);
    PLPLOT::TabA<double> vx = PLPLOT::col2taba(x, nx);
    PLPLOT::TabA<double> vy = PLPLOT::col2taba(y, ny);
    return new bloc::Value(bloc::Bool(h->mesh(vx.data, vy.data, vz.data, (int) nx, (int) ny, (int) *a3.integer())));
  }

  case PLPLOT::Meshc:
  {
    bloc::Value& a0 = args[0]->value(ctx);
    bloc::Value& a1 = args[1]->value(ctx);
    bloc::Value& a2 = args[2]->value(ctx);
    bloc::Value& a3 = args[3]->value(ctx);
    bloc::Value& a4 = args[4]->value(ctx);
    if (a0.isNull() || a1.isNull() || a2.isNull() || a3.isNull() || a4.isNull())
      throw RuntimeError(EXC_RT_OTHER_S, "Invalid arguments.");
    bloc::Collection& x = *a0.collection();
    bloc::Collection& y = *a1.collection();
    bloc::Collection& z = *a2.collection();
    bloc::Collection& c = *a4.collection();
    size_t nx = x.size();
    size_t ny = y.size();
    size_t nc = c.size();
    PLPLOT::TabZ<double> vz = PLPLOT::col2tabz(z, nx, ny);
    PLPLOT::TabA<double> vx = PLPLOT::col2taba(x, nx);
    PLPLOT::TabA<double> vy = PLPLOT::col2taba(y, ny);
    PLPLOT::TabA<double> vc = PLPLOT::col2taba(c, nc);
    return new bloc::Value(bloc::Bool(h->meshc(vx.data, vy.data, vz.data, (int) nx, (int) ny,
                                              (int) *a3.integer(), vc.data, nc)));
  }

  case PLPLOT::Scmap1n:
  {
    bloc::Value& a0 = args[0]->value(ctx);
    if (a0.isNull())
      throw RuntimeError(EXC_RT_OTHER_S, "Invalid arguments.");
    return new bloc::Value(bloc::Bool(h->scmap1n((int) *a0.integer())));
  }

  case PLPLOT::Scmap1l:
  {
    bloc::Value& a0 = args[0]->value(ctx);
    bloc::Value& a1 = args[1]->value(ctx);
    bloc::Value& a2 = args[2]->value(ctx);
    bloc::Value& a3 = args[3]->value(ctx);
    bloc::Value& a4 = args[4]->value(ctx);
    bloc::Value& a5 = args[5]->value(ctx);
    if (a0.isNull() || a1.isNull() || a2.isNull() || a3.isNull() || a4.isNull()
             || a5.isNull())
      throw RuntimeError(EXC_RT_OTHER_S, "Invalid arguments.");
    bloc::Collection& in = *a1.collection();
    bloc::Collection& c1 = *a2.collection();
    bloc::Collection& c2 = *a3.collection();
    bloc::Collection& c3 = *a4.collection();
    bloc::Collection& ap = *a5.collection();
    size_t n = in.size();
    if (c1.size() < n)
      n = c1.size();
    if (c2.size() < n)
      n = c2.size();
    if (c3.size() < n)
      n = c3.size();
    if (ap.size() < (n - 1))
      n = ap.size() + 1;
    PLPLOT::TabA<double> vi = PLPLOT::col2taba(in, n);
    PLPLOT::TabA<double> v1 = PLPLOT::col2taba(c1, n);
    PLPLOT::TabA<double> v2 = PLPLOT::col2taba(c2, n);
    PLPLOT::TabA<double> v3 = PLPLOT::col2taba(c3, n);
    PLPLOT::TabA<bool> vp = PLPLOT::col2tabab(ap, n);
    return new bloc::Value(bloc::Bool(h->scmap1l(*a0.boolean(), n, vi.data, v1.data, v2.data, v3.data, vp.data)));
  }

  case PLPLOT::Scmap1l_1:
  {
    bloc::Value& a0 = args[0]->value(ctx);
    bloc::Value& a1 = args[1]->value(ctx);
    bloc::Value& a2 = args[2]->value(ctx);
    bloc::Value& a3 = args[3]->value(ctx);
    bloc::Value& a4 = args[4]->value(ctx);
    if (a0.isNull() || a1.isNull() || a2.isNull() || a3.isNull() || a4.isNull())
      throw RuntimeError(EXC_RT_OTHER_S, "Invalid arguments.");
    bloc::Collection& in = *a1.collection();
    bloc::Collection& c1 = *a2.collection();
    bloc::Collection& c2 = *a3.collection();
    bloc::Collection& c3 = *a4.collection();
    size_t n = in.size();
    if (c1.size() < n)
      n = c1.size();
    if (c2.size() < n)
      n = c2.size();
    if (c3.size() < n)
      n = c3.size();
    PLPLOT::TabA<double> vi = PLPLOT::col2taba(in, n);
    PLPLOT::TabA<double> v1 = PLPLOT::col2taba(c1, n);
    PLPLOT::TabA<double> v2 = PLPLOT::col2taba(c2, n);
    PLPLOT::TabA<double> v3 = PLPLOT::col2taba(c3, n);
    return new bloc::Value(bloc::Bool(h->scmap1l(*a0.boolean(), n, vi.data, v1.data, v2.data, v3.data, nullptr)));
  }

  case PLPLOT::Scmap1la:
  {
    bloc::Value& a0 = args[0]->value(ctx);
    bloc::Value& a1 = args[1]->value(ctx);
    bloc::Value& a2 = args[2]->value(ctx);
    bloc::Value& a3 = args[3]->value(ctx);
    bloc::Value& a4 = args[4]->value(ctx);
    bloc::Value& a5 = args[5]->value(ctx);
    bloc::Value& a6 = args[6]->value(ctx);
    if (a0.isNull() || a1.isNull() || a2.isNull() || a3.isNull() || a4.isNull()
             || a5.isNull() || a6.isNull())
      throw RuntimeError(EXC_RT_OTHER_S, "Invalid arguments.");
    bloc::Collection& in = *a1.collection();
    bloc::Collection& c1 = *a2.collection();
    bloc::Collection& c2 = *a3.collection();
    bloc::Collection& c3 = *a4.collection();
    bloc::Collection& al = *a5.collection();
    bloc::Collection& ap = *a6.collection();
    size_t n = in.size();
    if (c1.size() < n)
      n = c1.size();
    if (c2.size() < n)
      n = c2.size();
    if (c3.size() < n)
      n = c3.size();
    if (al.size() < n)
      n = al.size();
    if (ap.size() < (n - 1))
      n = ap.size() + 1;
    PLPLOT::TabA<double> vi = PLPLOT::col2taba(in, n);
    PLPLOT::TabA<double> v1 = PLPLOT::col2taba(c1, n);
    PLPLOT::TabA<double> v2 = PLPLOT::col2taba(c2, n);
    PLPLOT::TabA<double> v3 = PLPLOT::col2taba(c3, n);
    PLPLOT::TabA<double> va = PLPLOT::col2taba(al, n);
    PLPLOT::TabA<bool> vp = PLPLOT::col2tabab(ap, n);
    return new bloc::Value(bloc::Bool(h->scmap1la(*a0.boolean(), n, vi.data, v1.data, v2.data, v3.data, va.data, vp.data)));
  }

  case PLPLOT::Scmap1la_1:
  {
    bloc::Value& a0 = args[0]->value(ctx);
    bloc::Value& a1 = args[1]->value(ctx);
    bloc::Value& a2 = args[2]->value(ctx);
    bloc::Value& a3 = args[3]->value(ctx);
    bloc::Value& a4 = args[4]->value(ctx);
    bloc::Value& a5 = args[5]->value(ctx);
    if (a0.isNull() || a1.isNull() || a2.isNull() || a3.isNull() || a4.isNull()
             || a5.isNull())
      throw RuntimeError(EXC_RT_OTHER_S, "Invalid arguments.");
    bloc::Collection& in = *a1.collection();
    bloc::Collection& c1 = *a2.collection();
    bloc::Collection& c2 = *a3.collection();
    bloc::Collection& c3 = *a4.collection();
    bloc::Collection& al = *a5.collection();
    size_t n = in.size();
    if (c1.size() < n)
      n = c1.size();
    if (c2.size() < n)
      n = c2.size();
    if (c3.size() < n)
      n = c3.size();
    if (al.size() < n)
      n = al.size();
    PLPLOT::TabA<double> vi = PLPLOT::col2taba(in, n);
    PLPLOT::TabA<double> v1 = PLPLOT::col2taba(c1, n);
    PLPLOT::TabA<double> v2 = PLPLOT::col2taba(c2, n);
    PLPLOT::TabA<double> v3 = PLPLOT::col2taba(c3, n);
    PLPLOT::TabA<double> va = PLPLOT::col2taba(al, n);
    return new bloc::Value(bloc::Bool(h->scmap1la(*a0.boolean(), n, vi.data, v1.data, v2.data, v3.data, va.data, nullptr)));
  }

  case PLPLOT::String3:
  {
    bloc::Value& a0 = args[0]->value(ctx);
    bloc::Value& a1 = args[1]->value(ctx);
    bloc::Value& a2 = args[2]->value(ctx);
    bloc::Value& a3 = args[3]->value(ctx);
    if (a0.isNull() || a1.isNull() || a2.isNull() || a3.isNull())
      throw RuntimeError(EXC_RT_OTHER_S, "Invalid arguments.");
    bloc::Collection& x = *a0.collection();
    bloc::Collection& y = *a1.collection();
    bloc::Collection& z = *a2.collection();
    size_t n = (x.size() > y.size() ? y.size() : x.size());
    if (z.size() > n)
      n = z.size();
    PLPLOT::TabA<double> vx = PLPLOT::col2taba(x, n);
    PLPLOT::TabA<double> vy = PLPLOT::col2taba(y, n);
    PLPLOT::TabA<double> vz = PLPLOT::col2taba(z, n);
    return new bloc::Value(bloc::Bool(h->string3((int) n, vx.data, vy.data, vz.data, *a2.literal())));
  }

  case PLPLOT::Fill3:
  {
    bloc::Value& a0 = args[0]->value(ctx);
    bloc::Value& a1 = args[1]->value(ctx);
    bloc::Value& a2 = args[2]->value(ctx);
    if (a0.isNull() || a1.isNull() || a2.isNull())
      throw RuntimeError(EXC_RT_OTHER_S, "Invalid arguments.");
    bloc::Collection& x = *a0.collection();
    bloc::Collection& y = *a1.collection();
    bloc::Collection& z = *a2.collection();
    size_t n = (x.size() > y.size() ? y.size() : x.size());
    if (z.size() > n)
      n = z.size();
    PLPLOT::TabA<double> vx = PLPLOT::col2taba(x, n);
    PLPLOT::TabA<double> vy = PLPLOT::col2taba(y, n);
    PLPLOT::TabA<double> vz = PLPLOT::col2taba(z, n);
    return new bloc::Value(bloc::Bool(h->fill3((int) n, vx.data, vy.data, vz.data)));
  }

  case PLPLOT::Line3:
  {
    bloc::Value& a0 = args[0]->value(ctx);
    bloc::Value& a1 = args[1]->value(ctx);
    bloc::Value& a2 = args[2]->value(ctx);
    if (a0.isNull() || a1.isNull() || a2.isNull())
      throw RuntimeError(EXC_RT_OTHER_S, "Invalid arguments.");
    bloc::Collection& x = *a0.collection();
    bloc::Collection& y = *a1.collection();
    bloc::Collection& z = *a2.collection();
    size_t n = (x.size() > y.size() ? y.size() : x.size());
    if (z.size() > n)
      n = z.size();
    PLPLOT::TabA<double> vx = PLPLOT::col2taba(x, n);
    PLPLOT::TabA<double> vy = PLPLOT::col2taba(y, n);
    PLPLOT::TabA<double> vz = PLPLOT::col2taba(z, n);
    return new bloc::Value(bloc::Bool(h->line3((int) n, vx.data, vy.data, vz.data)));
  }

  case PLPLOT::Lightsource:
  {
    bloc::Value& a0 = args[0]->value(ctx);
    bloc::Value& a1 = args[1]->value(ctx);
    bloc::Value& a2 = args[2]->value(ctx);
    if (a0.isNull() || a1.isNull() || a2.isNull())
      throw RuntimeError(EXC_RT_OTHER_S, "Invalid arguments.");
    return new bloc::Value(bloc::Bool(h->lightsource(*a0.numeric(), *a1.numeric(), *a2.numeric())));
  }

  case PLPLOT::Mtex3:
  {
    bloc::Value& a0 = args[0]->value(ctx);
    bloc::Value& a1 = args[1]->value(ctx);
    bloc::Value& a2 = args[2]->value(ctx);
    bloc::Value& a3 = args[3]->value(ctx);
    bloc::Value& a4 = args[4]->value(ctx);
    if (a0.isNull() || a1.isNull() || a2.isNull() || a3.isNull() || a4.isNull())
      throw RuntimeError(EXC_RT_OTHER_S, "Invalid arguments.");
    return new bloc::Value(bloc::Bool(h->mtex3(*a0.literal(), *a1.numeric(), *a2.numeric(),
                                               *a3.numeric(), *a4.literal())));
  }

  case PLPLOT::Poin:
  {
    bloc::Value& a0 = args[0]->value(ctx);
    bloc::Value& a1 = args[1]->value(ctx);
    bloc::Value& a2 = args[2]->value(ctx);
    if (a0.isNull() || a1.isNull() || a2.isNull())
      throw RuntimeError(EXC_RT_OTHER_S, "Invalid arguments.");
    bloc::Collection& x = *a0.collection();
    bloc::Collection& y = *a1.collection();
    size_t n = (x.size() > y.size() ? y.size() : x.size());
    PLPLOT::TabA<double> vx = PLPLOT::col2taba(x, n);
    PLPLOT::TabA<double> vy = PLPLOT::col2taba(y, n);
    return new bloc::Value(bloc::Bool(h->poin((int) n, vx.data, vy.data, (int) *a2.integer())));
  }

  case PLPLOT::Poin3:
  {
    bloc::Value& a0 = args[0]->value(ctx);
    bloc::Value& a1 = args[1]->value(ctx);
    bloc::Value& a2 = args[2]->value(ctx);
    bloc::Value& a3 = args[3]->value(ctx);
    if (a0.isNull() || a1.isNull() || a2.isNull() || a3.isNull())
      throw RuntimeError(EXC_RT_OTHER_S, "Invalid arguments.");
    bloc::Collection& x = *a0.collection();
    bloc::Collection& y = *a1.collection();
    bloc::Collection& z = *a2.collection();
    size_t n = (x.size() > y.size() ? y.size() : x.size());
    if (z.size() > n)
      n = z.size();
    PLPLOT::TabA<double> vx = PLPLOT::col2taba(x, n);
    PLPLOT::TabA<double> vy = PLPLOT::col2taba(y, n);
    PLPLOT::TabA<double> vz = PLPLOT::col2taba(z, n);
    return new bloc::Value(bloc::Bool(h->poin3((int) n, vx.data, vy.data, vz.data, (int) *a3.integer())));
  }

  case PLPLOT::Ptex3:
  {
    bloc::Value& a0 = args[0]->value(ctx);
    bloc::Value& a1 = args[1]->value(ctx);
    bloc::Value& a2 = args[2]->value(ctx);
    bloc::Value& a3 = args[3]->value(ctx);
    bloc::Value& a4 = args[4]->value(ctx);
    bloc::Value& a5 = args[5]->value(ctx);
    bloc::Value& a6 = args[6]->value(ctx);
    bloc::Value& a7 = args[7]->value(ctx);
    bloc::Value& a8 = args[8]->value(ctx);
    bloc::Value& a9 = args[9]->value(ctx);
    bloc::Value& a10 = args[10]->value(ctx);
    if (a0.isNull() || a1.isNull() || a2.isNull() || a3.isNull() || a4.isNull()
             || a5.isNull() || a6.isNull() || a7.isNull() || a8.isNull()
             || a9.isNull() || a10.isNull())
      throw RuntimeError(EXC_RT_OTHER_S, "Invalid arguments.");
    double wx = (a0.type() == bloc::Type::NUMERIC ? *a0.numeric() : *a0.integer());
    double wy = (a1.type() == bloc::Type::NUMERIC ? *a1.numeric() : *a1.integer());
    double wz = (a2.type() == bloc::Type::NUMERIC ? *a2.numeric() : *a2.integer());
    double dx = (a3.type() == bloc::Type::NUMERIC ? *a3.numeric() : *a3.integer());
    double dy = (a4.type() == bloc::Type::NUMERIC ? *a4.numeric() : *a4.integer());
    double dz = (a5.type() == bloc::Type::NUMERIC ? *a5.numeric() : *a5.integer());
    double sx = (a6.type() == bloc::Type::NUMERIC ? *a6.numeric() : *a6.integer());
    double sy = (a7.type() == bloc::Type::NUMERIC ? *a7.numeric() : *a7.integer());
    double sz = (a8.type() == bloc::Type::NUMERIC ? *a8.numeric() : *a8.integer());
    double just = (a9.type() == bloc::Type::NUMERIC ? *a9.numeric() : *a9.integer());
    return new bloc::Value(bloc::Bool(h->ptex3(wx, wy, wz, dx, dy, dz, sx, sy, sz,
                                             just, *a10.literal())));
  }

  case PLPLOT::Plot3d:
  {
    bloc::Value& a0 = args[0]->value(ctx);
    bloc::Value& a1 = args[1]->value(ctx);
    bloc::Value& a2 = args[2]->value(ctx);
    bloc::Value& a3 = args[3]->value(ctx);
    bloc::Value& a4 = args[4]->value(ctx);
    if (a0.isNull() || a1.isNull() || a2.isNull() || a3.isNull() || a4.isNull())
      throw RuntimeError(EXC_RT_OTHER_S, "Invalid arguments.");
    bloc::Collection& x = *a0.collection();
    bloc::Collection& y = *a1.collection();
    bloc::Collection& z = *a2.collection();
    size_t nx = x.size();
    size_t ny = y.size();
    PLPLOT::TabZ<double> vz = PLPLOT::col2tabz(z, nx, ny);
    PLPLOT::TabA<double> vx = PLPLOT::col2taba(x, nx);
    PLPLOT::TabA<double> vy = PLPLOT::col2taba(y, ny);
    return new bloc::Value(bloc::Bool(h->plot3(vx.data, vy.data, vz.data, (int) nx, (int) ny,
                                               (int) *a3.integer(), *a4.boolean())));
  }

  case PLPLOT::Plot3dc:
  {
    bloc::Value& a0 = args[0]->value(ctx);
    bloc::Value& a1 = args[1]->value(ctx);
    bloc::Value& a2 = args[2]->value(ctx);
    bloc::Value& a3 = args[3]->value(ctx);
    bloc::Value& a4 = args[4]->value(ctx);
    if (a0.isNull() || a1.isNull() || a2.isNull() || a3.isNull() || a4.isNull())
      throw RuntimeError(EXC_RT_OTHER_S, "Invalid arguments.");
    bloc::Collection& x = *a0.collection();
    bloc::Collection& y = *a1.collection();
    bloc::Collection& z = *a2.collection();
    bloc::Collection& c = *a4.collection();
    size_t nx = x.size();
    size_t ny = y.size();
    size_t nc = c.size();
    PLPLOT::TabZ<double> vz = PLPLOT::col2tabz(z, nx, ny);
    PLPLOT::TabA<double> vx = PLPLOT::col2taba(x, nx);
    PLPLOT::TabA<double> vy = PLPLOT::col2taba(y, ny);
    PLPLOT::TabA<double> vc = PLPLOT::col2taba(c, nc);
    return new bloc::Value(bloc::Bool(h->plot3c(vx.data, vy.data, vz.data, (int) nx, (int) ny,
                                               (int) *a3.integer(), vc.data, (int) nc)));
  }

  case PLPLOT::Prec:
  {
    bloc::Value& a0 = args[0]->value(ctx);
    bloc::Value& a1 = args[1]->value(ctx);
    if (a0.isNull() || a1.isNull())
      throw RuntimeError(EXC_RT_OTHER_S, "Invalid arguments.");
    return new bloc::Value(bloc::Bool(h->prec((int) *a0.integer(), (int) *a1.integer())));
  }

  case PLPLOT::Surf3d:
  {
    bloc::Value& a0 = args[0]->value(ctx);
    bloc::Value& a1 = args[1]->value(ctx);
    bloc::Value& a2 = args[2]->value(ctx);
    bloc::Value& a3 = args[3]->value(ctx);
    bloc::Value& a4 = args[4]->value(ctx);
    if (a0.isNull() || a1.isNull() || a2.isNull() || a3.isNull() || a4.isNull())
      throw RuntimeError(EXC_RT_OTHER_S, "Invalid arguments.");
    bloc::Collection& x = *a0.collection();
    bloc::Collection& y = *a1.collection();
    bloc::Collection& z = *a2.collection();
    bloc::Collection& c = *a4.collection();
    size_t nx = x.size();
    size_t ny = y.size();
    size_t nc = c.size();
    PLPLOT::TabZ<double> vz = PLPLOT::col2tabz(z, nx, ny);
    PLPLOT::TabA<double> vx = PLPLOT::col2taba(x, nx);
    PLPLOT::TabA<double> vy = PLPLOT::col2taba(y, ny);
    PLPLOT::TabA<double> vc = PLPLOT::col2taba(c, nc);
    return new bloc::Value(bloc::Bool(h->surf3(vx.data, vy.data, vz.data, (int) nx, (int) ny,
                                               (int) *a3.integer(), vc.data, (int) nc)));
  }

  case PLPLOT::Surf3dl:
  {
    bloc::Value& a0 = args[0]->value(ctx);
    bloc::Value& a1 = args[1]->value(ctx);
    bloc::Value& a2 = args[2]->value(ctx);
    bloc::Value& a3 = args[3]->value(ctx);
    bloc::Value& a4 = args[4]->value(ctx);
    bloc::Value& a5 = args[5]->value(ctx);
    bloc::Value& a6 = args[6]->value(ctx);
    bloc::Value& a7 = args[7]->value(ctx);
    bloc::Value& a8 = args[8]->value(ctx);
    if (a0.isNull() || a1.isNull() || a2.isNull() || a3.isNull() || a4.isNull()
            || a5.isNull() || a6.isNull() || a7.isNull() || a8.isNull())
      throw RuntimeError(EXC_RT_OTHER_S, "Invalid arguments.");
    bloc::Collection& x = *a0.collection();
    bloc::Collection& y = *a1.collection();
    bloc::Collection& z = *a2.collection();
    bloc::Collection& c = *a4.collection();
    int xmin = (int) *a5.integer();
    int xmax = (int) *a6.integer();
    bloc::Collection& ymin = *a7.collection();
    bloc::Collection& ymax = *a8.collection();
    size_t nx = x.size();
    size_t ny = y.size();
    size_t nc = c.size();
    PLPLOT::TabZ<double> vz = PLPLOT::col2tabz(z, nx, ny);
    PLPLOT::TabA<double> vx = PLPLOT::col2taba(x, nx);
    PLPLOT::TabA<double> vy = PLPLOT::col2taba(y, ny);
    PLPLOT::TabA<double> vc = PLPLOT::col2taba(c, (nc > 0 ? nc : 1));
    PLPLOT::TabA<int> vymin = PLPLOT::col2tabai(ymin, nx);
    PLPLOT::TabA<int> vymax = PLPLOT::col2tabai(ymax, nx);
    return new bloc::Value(bloc::Bool(h->surf3l(vx.data, vy.data, vz.data, (int) nx, (int) ny,
                                                (int) *a3.integer(), vc.data, (int) nc,
                                                xmin, xmax, vymin.data, vymax.data)));
  }

  case PLPLOT::Pat:
  {
    bloc::Value& a0 = args[0]->value(ctx);
    bloc::Value& a1 = args[1]->value(ctx);
    if (a0.isNull() || a1.isNull())
      throw RuntimeError(EXC_RT_OTHER_S, "Invalid arguments.");
    bloc::Collection& inc = *a0.collection();
    bloc::Collection& del = *a1.collection();
    size_t n = (inc.size() > del.size() ? del.size() : inc.size());
    PLPLOT::TabA<int> vi = PLPLOT::col2tabai(inc, n);
    PLPLOT::TabA<int> vd = PLPLOT::col2tabai(del, n);
    return new bloc::Value(bloc::Bool(h->pat((int) n, vi.data, vd.data)));
  }

  default:
    break;
  }
  return nullptr;
}

void PLPLOT::Handle::reset()
{
  close();
  _pls = new plstream((PLINT)_nx, (PLINT)_ny, _device.c_str(), _filename.c_str());
  _pls->spause(_paused);
  /* load customized colors */
  for(auto const& e : _palette)
    _pls->scol0a(e.first, e.second.r, e.second.g, e.second.b, e.second.a);
}

std::string PLPLOT::Handle::version()
{
  char buf[80];
  _pls->gver(buf);
  return std::string(buf);
}

bool PLPLOT::Handle::init(const std::string& args)
{
  int failed = 0;
  std::vector<std::pair<std::string, std::string> > options;
  parseoptions(args, options);
  reset();
  for(auto& opt : options)
  {
    /* discard exit */
    if (opt.first == "-h" || opt.first == "-v")
      continue;
    failed = (int) _pls->setopt(opt.first.c_str(), opt.second.c_str());
    if (failed != 0)
    {
      _msg.assign("Invalid option: ").append(opt.first).append(" = ").append(opt.second);
      return false;
    }
  }

  _pls->init();
  return true;
}

bool PLPLOT::Handle::close()
{
  if (_pls)
  {
    _pls->spause(0);
    delete _pls;
    _pls = nullptr;
  }
  return true;
}

bool PLPLOT::Handle::flush()
{
  _pls->flush();
  return true;
}

bool PLPLOT::Handle::replot()
{
  _pls->replot();
  return true;
}

bool PLPLOT::Handle::paused(bool enabled)
{
  _pls->spause(enabled);
  _paused = enabled;
  return enabled;
}

bool PLPLOT::Handle::env(double xmin, double xmax, double ymin, double ymax, int just, int axis)
{
  _pls->env((PLFLT) xmin, (PLFLT) xmax, (PLFLT) ymin, (PLFLT) ymax, (PLINT) just, (PLINT) axis);
  return true;
}

bool PLPLOT::Handle::lab(const std::string& xlabel, const std::string& ylabel, const std::string& tlabel)
{
  _pls->lab(xlabel.c_str(), ylabel.c_str(), tlabel.c_str());
  return true;
}

bool PLPLOT::Handle::line(int n, const double * x, const double * y)
{
  _pls->line((PLINT) n, (const PLFLT*) x, (const PLFLT*) y);
  return true;
}

bool PLPLOT::Handle::adv(int page)
{
  _pls->adv((PLINT) page);
  return true;
}

bool PLPLOT::Handle::vpor(double xmin, double xmax, double ymin, double ymax)
{
  _pls->vpor((PLFLT) xmin, (PLFLT) xmax, (PLFLT) ymin, (PLFLT) ymax);
  return true;
}

bool PLPLOT::Handle::wind(double xmin, double xmax, double ymin, double ymax)
{
  _pls->wind((PLFLT) xmin, (PLFLT) xmax, (PLFLT) ymin, (PLFLT) ymax);
  return true;
}

bool PLPLOT::Handle::width(double width)
{
  _pls->width((PLFLT) width);
  return true;
}

bool PLPLOT::Handle::col0(int color)
{
  _pls->col0((PLINT) color);
  return true;
}

bool PLPLOT::Handle::col1(double col1)
{
  _pls->col1((PLFLT) col1);
  return true;
}

bool PLPLOT::Handle::scolbga(int r, int g, int b, double alpha)
{
  _pls->scolbga((PLINT) r, (PLINT) g, (PLINT) b, (PLFLT) alpha);
  _palette.erase((PLINT)0);
  _palette.insert(std::make_pair((PLINT)0, rgba_t({r, g, b, alpha})));
  return true;
}

bool PLPLOT::Handle::scol0a(int col0, int r, int g, int b, double alpha)
{
  _pls->scol0a((PLINT) col0, (PLINT) r, (PLINT) g, (PLINT) b, (PLFLT) alpha);
  _palette.erase((PLINT)col0);
  _palette.insert(std::make_pair((PLINT)col0, rgba_t({r, g, b, alpha})));
  return true;
}

bool PLPLOT::Handle::ptex(double x, double y, double dx, double dy, double just, const std::string& text)
{
  _pls->ptex((PLFLT) x, (PLFLT) y, (PLFLT) dx, (PLFLT) dy, (PLFLT) just, text.c_str());
  return true;
}

bool PLPLOT::Handle::mtex(const std::string& side, double disp, double pos, double just, const std::string& text)
{
  _pls->mtex(side.c_str(), (PLFLT) disp, (PLFLT) pos, (PLFLT) just, text.c_str());
  return true;
}

bool PLPLOT::Handle::box(const std::string& xopt, double xtick, int nxsub, const std::string& yopt, double ytick, int nysub)
{
  _pls->box(xopt.c_str(), (PLFLT) xtick, (PLINT) nxsub, yopt.c_str(), (PLFLT) ytick, (PLINT) nysub);
  return true;
}

bool PLPLOT::Handle::schr(double def, double scale)
{
  _pls->schr((PLFLT) def, (PLFLT) scale);
  return true;
}

bool PLPLOT::Handle::string(int n, const double * x, const double * y, const std::string& str)
{
  _pls->string((PLINT) n, (const PLFLT*) x, (const PLFLT*) y, str.c_str());
  return true;
}

bool PLPLOT::Handle::axes(double x0, double y0, const std::string& xopt, double xtick, int nxsub, const std::string& yopt, double ytick, int nysub)
{
  _pls->axes((PLFLT) x0, (PLFLT) y0, xopt.c_str(), (PLFLT) xtick, (PLINT) nxsub,
          yopt.c_str(), (PLFLT) ytick, (PLINT) nysub);
  return true;
}

bool PLPLOT::Handle::bin(int nbin, const double * x, const double * y, int center)
{
  _pls->bin((PLINT) nbin, (const PLFLT*) x, (const PLFLT*) y, (PLINT) center);
  return true;
}

bool PLPLOT::Handle::errx(int n, const double * xmin, const double * xmax, const double * y)
{
  _pls->errx((PLINT) n, (const PLFLT*) xmin, (const PLFLT*) xmax, (const PLFLT*) y);
  return true;
}

bool PLPLOT::Handle::erry(int n, const double * x, const double * ymin, const double * ymax)
{
  _pls->erry((PLINT) n, (const PLFLT*) x, (const PLFLT*) ymin, (const PLFLT*) ymax);
  return true;
}

bool PLPLOT::Handle::fill(int n, const double * x, const double * y)
{
  _pls->fill((PLINT) n, (const PLFLT*) x, (const PLFLT*) y);
  return true;
}

bool PLPLOT::Handle::font(int ifont)
{
  _pls->font((PLINT) ifont);
  return true;
}

bool PLPLOT::Handle::lsty(int lin)
{
  _pls->lsty((PLINT) lin);
  return true;
}


bool PLPLOT::Handle::minmax2dgrid(const double * const * z, int nx, int ny,
                                  double * zmax, double * zmin )
{
  _pls->MinMax2dGrid((const PLFLT* const*) z, (PLINT) nx, (PLINT) ny, (PLFLT*) zmax, (PLFLT*) zmin);
  return true;
}

bool PLPLOT::Handle::w3d(double basex, double basey, double height,
                         double xmin, double xmax, double ymin, double ymax,
                         double zmin, double zmax, double alt, double az)
{
  _pls->w3d((PLFLT) basex, (PLFLT) basey, (PLFLT) height, (PLFLT) xmin, (PLFLT) xmax,
            (PLFLT) ymin, (PLFLT) ymax, (PLFLT) zmin, (PLFLT) zmax, (PLFLT) alt, (PLFLT) az);
  return true;
}

bool PLPLOT::Handle::box3(const std::string& xopt, const std::string& xlabel, double xtick, int nxsub,
                          const std::string& yopt, const std::string& ylabel, double ytick, int nysub,
                          const std::string& zopt, const std::string& zlabel, double ztick, int nzsub)
{
  _pls->box3(xopt.c_str(), xlabel.c_str(), (PLFLT) xtick, (PLINT) nxsub,
             yopt.c_str(), ylabel.c_str(), (PLFLT) ytick, (PLINT) nysub,
             zopt.c_str(), zlabel.c_str(), (PLFLT) ztick, (PLINT) nzsub);
  return true;
}

bool PLPLOT::Handle::mesh(const double * x, const double * y, const double * const * z,
                          int nx, int ny, int opt)
{
  _pls->mesh((const PLFLT*) x, (const PLFLT*) y, (const PLFLT* const*) z, (PLINT) nx, (PLINT) ny, (PLINT) opt);
  return true;
}

bool PLPLOT::Handle::meshc(const double * x, const double * y, const double * const * z,
                           int nx, int ny, int opt, const double * clevel, int nlevel)
{
  if (nlevel == 0)
    clevel = nullptr;
  _pls->meshc((const PLFLT*) x, (const PLFLT*) y, (const PLFLT* const*) z, (PLINT) nx, (PLINT) ny, (PLINT) opt,
             (const PLFLT*) clevel, (PLINT) nlevel);
  return true;
}

bool PLPLOT::Handle::scmap1n(int ncol1)
{
  _pls->scmap1n((PLINT) ncol1);
  return true;
}

bool PLPLOT::Handle::scmap1l(bool itype, int npts, const double * intensity, const double * coord1, const double * coord2,
        const double * coord3, const bool * alt_hue_path)
{
  _pls->scmap1l(itype, (PLINT) npts, (const PLFLT*) intensity, (const PLFLT*) coord1, (const PLFLT*) coord2,
        (const PLFLT*) coord3, alt_hue_path);
  return true;
}

bool PLPLOT::Handle::scmap1la(bool itype, int npts, const double * intensity, const double * coord1, const double * coord2,
        const double * coord3, const double * alpha, const bool * alt_hue_path)
{
  _pls->scmap1la(itype, (PLINT) npts, (const PLFLT*) intensity, (const PLFLT*) coord1, (const PLFLT*) coord2,
        (const PLFLT*) alpha, (const PLFLT*) coord3, alt_hue_path);
  return true;
}

bool PLPLOT::Handle::string3(int n, const double * x, const double * y, const double * z, const std::string& str)
{
  _pls->string3((PLINT) n, (const PLFLT*) x, (const PLFLT*) y, (const PLFLT*) z, str.c_str());
  return true;
}

bool PLPLOT::Handle::fill3(int n, const double * x, const double * y, const double * z)
{
  _pls->fill3((PLINT) n, (const PLFLT*) x, (const PLFLT*) y, (const PLFLT*) z);
  return true;
}

bool PLPLOT::Handle::line3(int n, const double * x, const double * y, const double * z)
{
  _pls->line3((PLINT) n, (const PLFLT*) x, (const PLFLT*) y, (const PLFLT*) z);
  return true;
}

bool PLPLOT::Handle::lightsource(double x, double y, double z)
{
  _pls->lightsource((PLFLT) x, (PLFLT) y, (PLFLT) z);
  return true;
}

bool PLPLOT::Handle::mtex3(const std::string& side, double disp, double pos, double just, const std::string& text)
{
  _pls->mtex3(side.c_str(), (PLFLT) disp, (PLFLT) pos, (PLFLT) just, text.c_str());
  return true;
}

bool PLPLOT::Handle::poin(int n, const double * x, const double * y, int code)
{
  _pls->poin((PLINT) n, (const PLFLT*) x, (const PLFLT*) y, (PLINT) code);
  return true;
}

bool PLPLOT::Handle::poin3(int n, const double * x, const double * y, const double * z, int code)
{
  _pls->poin3((PLINT) n, (const PLFLT*) x, (const PLFLT*) y, (const PLFLT*) z, (PLINT) code);
  return true;
}

bool PLPLOT::Handle::ptex3(double wx, double wy, double wz, double dx, double dy, double dz,
                           double sx, double sy, double sz, double just, const std::string& text)
{
  _pls->ptex3((PLFLT) wx, (PLFLT) wy, (PLFLT) wz, (PLFLT) dx, (PLFLT) dy, (PLFLT) dz,
              (PLFLT) sx, (PLFLT) sy, (PLFLT) sz, (PLFLT) just, text.c_str());
  return true;
}

bool PLPLOT::Handle::plot3(const double * x, const double * y, const double * const * z,
                          int nx, int ny, int opt, bool side)
{
  _pls->plot3d((const PLFLT*) x, (const PLFLT*) y, (const PLFLT* const*) z, (PLINT) nx, (PLINT) ny,
               (PLINT) opt, side);
  return true;
}

bool PLPLOT::Handle::plot3c(const double * x, const double * y, const double * const * z, int nx,
                            int ny, int opt, const double * clevel, int nlevel)
{
  if (nlevel == 0)
    clevel = nullptr;
  _pls->plot3dc((const PLFLT*) x, (const PLFLT*) y, (const PLFLT* const*) z, (PLINT) nx, (PLINT) ny,
               (PLINT) opt, (const PLFLT*) clevel, (PLINT) nlevel);
  return true;
}

bool PLPLOT::Handle::prec(int setp, int prec)
{
  _pls->prec((PLINT) setp, (PLINT) prec);
  return true;
}

bool PLPLOT::Handle::surf3(const double * x, const double * y, const double * const * z, int nx,
                           int ny, int opt, const double * clevel, int nlevel)
{
  if (nlevel == 0)
    clevel = nullptr;
  _pls->surf3d((const PLFLT*) x, (const PLFLT*) y, (const PLFLT* const*) z, (PLINT) nx, (PLINT) ny,
                (PLINT) opt, (const PLFLT*) clevel, (PLINT) nlevel);
  return true;
}

bool PLPLOT::Handle::surf3l(const double * x, const double * y, const double * const * z, int nx,
                            int ny, int opt, const double * clevel, int nlevel,
                            int indexxmin, int indexxmax, const int * indexymin, const int * indexymax)
{
  if (nlevel == 0)
    clevel = nullptr;
  _pls->surf3dl((const PLFLT*) x, (const PLFLT*) y, (const PLFLT* const*) z, (PLINT) nx, (PLINT) ny,
                 (PLINT) opt, (const PLFLT*) clevel, (PLINT) nlevel,
                 (PLINT) indexxmin, (PLINT) indexxmax, (const PLINT*) indexymin, (const PLINT*) indexymax);
  return true;
}

bool PLPLOT::Handle::pat(int nlin, const int * inc, const int * del)
{
  _pls->pat((PLINT) nlin, (const PLINT*) inc, (const PLINT*) del);
  return true;
}

} /* namespace import */
} /* namespace bloc */
