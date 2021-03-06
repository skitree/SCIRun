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

#include <Interface/Modules/Math/PlotDialog.h>
#include <Core/Datatypes/DenseMatrix.h>
#include <qwt_plot_marker.h>
#include <qwt_plot_curve.h>
#include <qwt_legend.h>
#include <qwt_legend_label.h>
#include <qwt_point_data.h>
#include <qwt_plot_canvas.h>
#include <qwt_plot_panner.h>
#include <qwt_plot_magnifier.h>
#include <qwt_text.h>
#include <qwt_symbol.h>
#include <qwt_math.h>
#include <qwt_plot_renderer.h>
#ifndef Q_MOC_RUN
#include <Core/Utils/StringUtil.h>
#endif

using namespace SCIRun::Gui;
using namespace SCIRun::Core::Datatypes;

PlotDialog::PlotDialog(QWidget* parent)
{
  setStyleSheet(styleSheet());
  auto layout = new QVBoxLayout;
  layout->setContentsMargins( 5, 5, 5, 5 );

  auto zoomBox = new QComboBox(parent);
  zoomBox->addItem("Zoom both axes");
  zoomBox->addItem("Zoom vertical axis");
  zoomBox->addItem("Zoom horizontal axis");
  zoomBox->setMaximumWidth(180);
  layout->addWidget(zoomBox);

  plot_ = new Plot(parent);
  layout->addWidget( plot_ );

  connect(zoomBox, SIGNAL(activated(const QString&)), plot_, SLOT(adjustZoom(const QString&)));

  setLayout(layout);
  resize( 600, 400 );
  move(10, 10);
}

PlotDialog::~PlotDialog()
{
  delete plot_;
}

void PlotDialog::updatePlot(const QString& title, const QString& xAxis, const QString& yAxis,
  const boost::optional<double>& horizAxisOpt,
  const boost::optional<double>& vertAxisOpt)
{
  plot_->setTitle(title);
  plot_->setAxisTitle(QwtPlot::xBottom, xAxis);
  plot_->setAxisTitle(QwtPlot::yLeft, yAxis);
  plot_->makeHorizontalAxis(static_cast<bool>(horizAxisOpt), horizAxisOpt.value_or(0));
  plot_->makeVerticalAxis(static_cast<bool>(vertAxisOpt), vertAxisOpt.value_or(0));
  plot_->replot();
}

Plot::Plot(QWidget *parent) : QwtPlot( parent )
{
  setAutoFillBackground( true );

  auto canvas = new QwtPlotCanvas(this);
  canvas->setLineWidth( 1 );
  canvas->setFrameStyle( QFrame::Box | QFrame::Plain );
  canvas->setBorderRadius( 15 );

  setCanvas( canvas );

  // panning with the left mouse button
  ( void ) new QwtPlotPanner( canvas );

  // zoom in/out with the wheel
  magnifier_ = new QwtPlotMagnifier( canvas );

	setAutoReplot(true);
}

void Plot::adjustZoom(const QString& type)
{
  auto zoomVertical = type.contains("both") || type.contains("vertical");
  auto zoomHorizontal = type.contains("both") || type.contains("horizontal");
  magnifier_->setAxisEnabled(QwtPlot::xBottom, zoomHorizontal);
  magnifier_->setAxisEnabled(QwtPlot::yLeft, zoomVertical);
}

void Plot::addLegend()
{
	auto legend = new QwtLegend();
	legend->setDefaultItemMode(QwtLegendData::Checkable);
  insertLegend(legend, QwtPlot::RightLegend);
	connect(legend, SIGNAL(checked(const QVariant&, bool, int)), SLOT(showItem(const QVariant&, bool)));

	auto items = itemList( QwtPlotItem::Rtti_PlotCurve );
	for ( int i = 0; i < items.size(); i++ )
	{
		const QVariant itemInfo = itemToInfo( items[i] );
		auto legendLabel = dynamic_cast<QwtLegendLabel*>(legend->legendWidget(itemInfo));
		if (legendLabel)
			legendLabel->setChecked( true );
	}
}

void Plot::showItem(const QVariant& itemInfo, bool on)
{
  auto plotItem = infoToItem(itemInfo);
  if (plotItem)
	{
    plotItem->setVisible(on);
	}
}

void Plot::makeVerticalAxis(bool show, double position)
{
	if (show)
	{
		delete verticalAxis_;
		verticalAxis_ = new QwtPlotMarker();
		verticalAxis_->setLabel("x = " + QString::number(position));
		verticalAxis_->setLabelAlignment( Qt::AlignLeft | Qt::AlignBottom );
		verticalAxis_->setLabelOrientation( Qt::Vertical );
		verticalAxis_->setLineStyle( QwtPlotMarker::VLine );
		verticalAxis_->setLinePen( Qt::black, 0, Qt::DashDotLine );
		verticalAxis_->setXValue( position );
		verticalAxis_->attach( this );
	}
	else
	{
		if (verticalAxis_)
			verticalAxis_->detach();
	}
}

void Plot::makeHorizontalAxis(bool show, double position)
{
	if (show)
	{
		delete horizontalAxis_;
		horizontalAxis_ = new QwtPlotMarker();
		horizontalAxis_->setLabel("y = " + QString::number(position));
		horizontalAxis_->setLabelAlignment( Qt::AlignRight | Qt::AlignTop );
		horizontalAxis_->setLineStyle( QwtPlotMarker::HLine );
		horizontalAxis_->setYValue( position );
		horizontalAxis_->attach( this );
	}
	else
	{
		if (horizontalAxis_)
			horizontalAxis_->detach();
	}
}

void Plot::addCurve(DenseMatrixHandle data, const QString& title, const QColor& color, bool showLegend, bool showPoints)
{
	addCurve(data->col(0), data->col(1), title, color, showLegend, showPoints);
}

void Plot::addCurveImpl(const QPolygonF& points, const QString& title, const QColor& color, bool showLegend, bool showPoints)
{
  auto curve = new QwtPlotCurve();
	curves_.push_back(curve);
  curve->setPen(color, 2),
  curve->setTitle(title);
  curve->setRenderHint( QwtPlotItem::RenderAntialiased, true );
	curve->setLegendAttribute( QwtPlotCurve::LegendShowLine, showLegend );
	if (showPoints)
		curve->setSymbol(new QwtSymbol(QwtSymbol::Ellipse, Qt::yellow, QPen(Qt::blue), QSize(3, 3)));
  curve->attach(this);
  curve->setSamples( points );
  updateCurveStyle(curve);
}

void Plot::clearCurves()
{
	for (auto& curve : curves_)
	{
		curve->detach();
		delete curve;
	}
	curves_.clear();
}

void Plot::exportPlot()
{
  QwtPlotRenderer renderer;
	renderer.exportTo(this, "scirunplot.pdf");
}

void Plot::setCurveStyle(const QString& style)
{
  curveStyle_ = style;
  for (auto& curve : curves_)
	{
    updateCurveStyle(curve);
  }
}

void Plot::updateCurveStyle(QwtPlotCurve* curve)
{
  if (curveStyle_ == "Lines")
    curve->setStyle(QwtPlotCurve::Lines);
  else if (curveStyle_ == "Steps")
    curve->setStyle(QwtPlotCurve::Steps);
  else if (curveStyle_ == "None")
    curve->setStyle(QwtPlotCurve::NoCurve);
  else if (curveStyle_ == "Dots")
    curve->setStyle(QwtPlotCurve::Dots);
}
