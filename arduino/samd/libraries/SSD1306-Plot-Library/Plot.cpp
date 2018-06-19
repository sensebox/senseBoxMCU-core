#include "Plot.h"
#include <avr/dtostrf.h>


Plot::Plot(Adafruit_SSD1306 *display) {
    _display = display;
}

// Accessors {{{

void Plot::setPlotSize(double x, double y, double w, double h) {
    _xpos = x;
    _ypos = y;
    _width = w;
    _height = h;
}

void Plot::setXRange(double xmin, double xmax) {
    _xmin = xmin;
    _xmax = xmax;
}

void Plot::setYRange(double ymin, double ymax) {
    _ymin = ymin;
    _ymax = ymax;
}

//void Plot::setYAutoRange(bool yautorange) {
//    _yautorange = yautoRange;
//}

void Plot::setXTick(double xtick) {
    _xtick = xtick;
}

void Plot::setYTick(double ytick) {
    _ytick = ytick;
}

void Plot::setXPrecision(unsigned int xprecision) {
    _xprecision = xprecision;
}

void Plot::setYPrecision(unsigned int yprecision) {
    _yprecision = yprecision;
}

void Plot::setTitle(String title) {
    _title = title;
}

void Plot::setXLabel(String xlabel) {
    _xlabel = xlabel;
}

void Plot::setYLabel(String ylabel) {
    _ylabel = ylabel;
}

void Plot::setForegroundColor(unsigned int fcolor) {
    _fcolor = fcolor;
}

void Plot::setBackroundColor(unsigned int bcolor) {
    _bcolor = bcolor;
}

void Plot::clear() {
    _haslast = false;
    _display->clearDisplay();
    drawPlot();
}

// }}}
// Plotting {{{

void Plot::addDataPoint(double x, double y) {
    double xg = domainToGraphX(x);
    double yg = domainToGraphY(y);
    if (_haslast) {
        double lastxg = domainToGraphX(_lastx);
        double lastyg = domainToGraphY(_lasty);
        _display->drawLine(lastxg, lastyg, xg, yg, _fcolor);
    } else {
        _haslast = true;
        _display->drawPixel(xg, yg, _fcolor);
    }
    _lastx = x;
    _lasty = y;
    _display->display();
}

void Plot::drawPlot() {
    _graphheight = _height - titleHeight() - xLabelHeight() - xValueHeight();
    _graphwidth = _width - yValueWidth() - yLabelWidth();
    _graphxpos = _xpos + yValueWidth() + yLabelWidth();
    _graphypos = _ypos - xLabelHeight() - xValueHeight();

    _display->setTextSize(TEXT_SIZE);
    _display->setTextColor(_fcolor, _bcolor);
    displayTitle();
    displayXLabel();
    displayYLabel();
    displayXValues();
    displayYValues();
    drawGrid();

    _display->display();
}

void Plot::displayTitle() {
    if (_title.length() > 0) {
        // Title should be drawn center-aligned over the graph
        _display->setCursor(_graphxpos + _graphwidth/2 - textpxwidth(_title, TEXT_SIZE)/2,
                           _graphypos - _graphheight - titleHeight());
        _display->println(_title); 
    }
}

void Plot::displayXLabel() {
    if (_xlabel.length() > 0) {
        // x-label should be drawn center-aligned below the x values
        _display->setCursor(_graphxpos + _graphwidth/2 - textpxwidth(_xlabel, TEXT_SIZE)/2,
                           _graphypos + xValueHeight());
        _display->println(_xlabel); 
    }
}

void Plot::displayYLabel() {
    if (_ylabel.length() > 0) {
        _display->setCursor(_xpos, _graphypos - _graphheight/2 - textpxheight(_ylabel, TEXT_SIZE)/2);
        _display->println(_ylabel);
    }
}

void Plot::displayXValues() {
    if (_xtick > 0) {
        for (double x = _xmin; x <= _xmax; x += _xtick) {
            String value = String(x, _xprecision);
            double xg = domainToGraphX(x);
            xg = xg - textpxwidth(value, TEXT_SIZE)/2;
            xg = max(_graphxpos, 
                    min(_graphxpos + _graphwidth - textpxwidth(value, TEXT_SIZE),
                        xg));
            _display->setCursor(xg, _graphypos + 1);
            _display->println(value); 
        }
    }
}

void Plot::displayYValues() {
    if (_ytick > 0) {
        for (double y = _ymin; y <= _ymax; y += _ytick) {
            double yg = domainToGraphY(y);
            String value = String(y, _yprecision);
            _display->setCursor(_graphxpos - textpxwidth(value, TEXT_SIZE), yg - textpxheight(value, TEXT_SIZE)/2);
            _display->println(value);
        }
    }
}

void Plot::drawGrid() {
    if (_ytick > 0) {
        // Draw horizontal grid lines
        for (double y = _ymin; y <= _ymax; y += _ytick) {
            double yg = domainToGraphY(y);
            _display->drawLine(_graphxpos, yg, _graphxpos + _graphwidth, yg, _fcolor);
        }
    }
    if (_xtick > 0) {
        // Draw vertical grid lines
        for (double x = _xmin; x <= _xmax; x += _xtick) {
            double xg = domainToGraphX(x);
            _display->drawLine((int)xg, _graphypos - _graphheight, (int)xg, _graphypos, _fcolor);
        }
    }
}

unsigned int Plot::titleHeight() {
    if (_title.length() > 0) {
        return textpxheight(_title, TEXT_SIZE);
    }
    return 0;
}

unsigned int Plot::xLabelHeight() {
    if (_xlabel.length() > 0) {
        return textpxheight(_xlabel, TEXT_SIZE);
    }
    return 0;
}

unsigned int Plot::yLabelWidth() {
    if (_ylabel.length() > 0) {
        return textpxwidth(_ylabel, TEXT_SIZE);
    }
    return 0;
}

unsigned int Plot::xValueHeight() {
    unsigned int maxheight = 0;
    if (_xtick > 0) {
        for (double x = _xmin; x <= _xmax; x += _xtick) {
            maxheight = max(maxheight, textpxheight(String(x, _xprecision), TEXT_SIZE) + 1);
        }
    }
    return maxheight;
}

unsigned int Plot::yValueWidth() {
    unsigned int maxwidth = 0;
    if (_ytick > 0) {
        for (double y = _ymin; y <= _ymax; y += _ytick) {
            maxwidth = max(maxwidth, textpxwidth(String(y, _yprecision), TEXT_SIZE));
        }
    }
    return maxwidth;
}

// }}}
// Util {{{

double Plot::domainToGraphX(double x) {
    return _graphxpos + ((x - _xmin) *  _graphwidth / (_xmax - _xmin));
}

double Plot::domainToGraphY(double y) {
    return _graphypos - ((y - _ymin) *  _graphheight / (_ymax - _ymin));
}

char *Plot::mydtostrf(float f, byte precision) {
  return removelpad(dtostrf(f, 30, precision, _floatStrBuf));
}

char *Plot::removelpad(char *str) {
    while(isspace((unsigned char)*str)) {
        str++;
    }
    return str;
}

unsigned int Plot::textpxwidth(String text, unsigned int fontsize) {
    return text.length() * charpxwidth(fontsize);
}

unsigned int Plot::textpxheight(String text, unsigned int fontsize) {
    return min(text.length(), 1) * charpxheight(fontsize);
}

unsigned int Plot::charpxwidth(unsigned int fontsize) {
    if (fontsize != 1) {
        Serial.println("Error: Char size calculation only implemented for font size of 1");
    }
    return 6;
}

unsigned int Plot::charpxheight(unsigned int fontsize) {
    if (fontsize != 1) {
        Serial.println("Error: Char size calculation only implemented for font size of 1");
    }
    return 8;
}
// }}}
// vim:foldmethod=marker:foldlevel=0
