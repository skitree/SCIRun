/*
For more information, please see: http://software.sci.utah.edu

The MIT License

Copyright (c) 2015 Scientific Computing and Imaging Institute,
University of Utah.

License for the specific language governing rights and limitations under
Permission is hereby granted, free of charge, to any person obtaining a
copy of this software and associated documentation files (the "Software"),
to deal in the Software without restriction, including without limitation
the rights to use, copy, modify, merge, publish, distribute, sublicense,
and/or sell copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included
in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
DEALINGS IN THE SOFTWARE.
*/

#include <Graphics/Widgets/Widget.h>
#include <Core/Datatypes/Geometry.h>
#include <Core/GeometryPrimitives/BBox.h>
#include <Graphics/Glyphs/GlyphGeom.h>

using namespace SCIRun;
using namespace Graphics;
using namespace Datatypes;
using namespace Core::Geometry;
using namespace Core::Datatypes;

Widget::Widget() : num_strips_(50)
{

}

void Widget::CreateBoundingBox(GeometryHandle geom, const Point& center, const Point& right, 
  const Point& down, const Point& in, const double scale, const Core::Geometry::BBox& bbox)
{
  ColorScheme colorScheme(COLOR_UNIFORM);
  std::vector<std::pair<Point, Point>> bounding_edges;
  //get all the bbox edges
  Vector x = right - center, y = down - center, z = in - center;
  std::vector<Point> points;
  points.resize(8);
  points.at(0) = center + x + y + z;
  points.at(1) = center + x + y - z;
  points.at(2) = center + x - y + z;
  points.at(3) = center + x - y - z;
  points.at(4) = center - x + y + z;
  points.at(5) = center - x + y - z;
  points.at(6) = center - x - y + z;
  points.at(7) = center - x - y - z;
  uint32_t point_indicies[] = {
    0, 1, 0, 2, 0, 4,
    7, 6, 7, 5, 3, 7,
    4, 5, 4, 6, 1, 5,
    3, 2, 3, 1, 2, 6
  };

  std::vector<Vector> tri_points;
  std::vector<Vector> tri_normals;
  std::vector<uint32_t> tri_indices;
  std::vector<ColorRGB> colors;
  GlyphGeom glyphs;
  //generate triangles for the cylinders.
  for (int edge = 0; edge < 24; edge += 2)
  {
    glyphs.addCylinder(points[point_indicies[edge]], points[point_indicies[edge + 1]], scale, num_strips_, ColorRGB(), ColorRGB());
  }
  //generate triangles for the spheres
  for (auto a : points)
  {
    glyphs.addSphere(a, scale, num_strips_, ColorRGB(1, 0, 0));
  }

  std::stringstream ss;
  ss << scale;
  for (auto a : points) ss << a.x() << a.y() << a.z();

  std::string uniqueNodeID = "bounding_box_cylinders" + std::string(ss.str().c_str());

  RenderState renState;

  renState.set(RenderState::IS_ON, true);
  renState.set(RenderState::USE_TRANSPARENCY, false);

  renState.defaultColor = ColorRGB(1, 1, 1);
  renState.set(RenderState::USE_DEFAULT_COLOR, true);
  renState.set(RenderState::USE_NORMALS, true);
  renState.set(RenderState::IS_WIDGET, true);

  glyphs.buildObject(geom, uniqueNodeID, renState.get(RenderState::USE_TRANSPARENCY), 1.0,
    colorScheme, renState, SpireIBO::TRIANGLES, bbox);
}

void Widget::CreateNode(GeometryHandle geom, const Point& center, const double scale, const Core::Geometry::BBox& bbox)
{
  ColorScheme colorScheme(COLOR_UNIFORM);

  GlyphGeom glyphs;
  
  glyphs.addSphere(center, scale, num_strips_, ColorRGB(1, 0, 0));

  std::stringstream ss;
  ss << scale;
  ss << center.x() << center.y() << center.z();

  std::string uniqueNodeID = "bounding_box_node" + std::string(ss.str().c_str());

  RenderState renState;

  renState.set(RenderState::IS_ON, true);
  renState.set(RenderState::USE_TRANSPARENCY, false);

  renState.defaultColor = ColorRGB(1, 1, 1);
  renState.set(RenderState::USE_DEFAULT_COLOR, true);
  renState.set(RenderState::USE_NORMALS, true);
  renState.set(RenderState::IS_WIDGET, true);

  glyphs.buildObject(geom, uniqueNodeID, renState.get(RenderState::USE_TRANSPARENCY), 1.0,
    colorScheme, renState, SpireIBO::TRIANGLES, bbox);

}