#include <Adafruit_SSD1306.h>

class Plot {

    public:
        //TODO [VAR_TEXT] Remove if variable text sizes are implemented and set default class member
        const unsigned int TEXT_SIZE = 1;
        Plot(Adafruit_SSD1306 *display);

        /**
         * Sets position and dimensions of the plot.
         * This always includes the labels.
         * Default is position (0, 64) and size (128, 64).
         */
        void setPlotSize(double x, double y, double w, double h);

        /**
         * Sets the displayed range of the x-axis.
         * xmax must be greater than or equal to xmin.
         */
        void setXRange(double xmin, double xmax);

        /**
         * Sets the displayed range of the y-axis.
         * ymax must be greater than or equal to ymin.
         */
        void setYRange(double ymin, double ymax);

        // TODO
        ///**
        // * Enables automatic range adjustemnt for the y-axis.
        // * With automatic range adjustemnt enabled the range of the y-axis
        // * will always be adjusted to fit all values.
        // * If the y-tick has not been set or is 0 (no ticks), 
        // * the range will be selected as the minimal and maximal value 
        // * of all datapoints and y-ticks will be placed equidistantly.
        // * Otherwise, the range will be rounded to match a y-tick and fit all data. 
        // */
        //void setYAutoRange(bool yautorange);
        
        //TODO Add line thickness option
        //void setLineThickness(unsigned int thickness);

        /**
         * Sets the distance between to x-ticks.
         * If it is set to 0 no ticks on the x-axis will be displayed.
         */
        void setXTick(double xtick);

        /**
         * Sets the distance between to y-ticks.
         * If it is set to 0 no ticks on the y-axis will be displayed.
         */
        void setYTick(double ytick);

        /**
         * Sets the decimal precision which is used to display the values at the x-axis
         */
        void setXPrecision(unsigned int xprecision);

        /**
         * Sets the decimal precision which is used to display the values at the y-axis
         */
        void setYPrecision(unsigned int yprecision);

        /**
         * Sets the title of the plot.
         * If an empty string is passed, no title will be set.
         */
        void setTitle(String title);

        /**
         * Sets the label of the x-axis of the plot.
         * If an empty string is passed, no label will be set.
         */
        void setXLabel(String xlabel);

        /**
         * Sets the label of the y-axis of the plot.
         * If an empty string is passed, no label will be set.
         */
        void setYLabel(String ylabel);

        /**
         * Sets the color in which all text and the plot will be drawn.
         * Default: WHITE
         */
        void setForegroundColor(unsigned int fcolor);

        /**
         * Sets the color of the background.
         * Default: BLACK
         */
        void setBackroundColor(unsigned int bcolor);

        /**
         * Draws the whole plot.
         * This function should be called if any changes are made,
         * which might need partial redrawing of the graph,
         * i.e. the change can not be applied by drawing with the foreground color over the existing graph.
         * The only action which needs no redraw of the whole plot at the moment is adding values
         * with greater x-coordinates than all other values.
         */
        void drawPlot();

        /**
         * Add one datapoint.
         */
        void addDataPoint(double x, double y);

        /**
         * Removes all data points. Needs redraw of plot.
         */
        void clear();

    private:
        Adafruit_SSD1306 *_display;
        double _xpos = 0;
        double _ypos = 64;
        double _width = 127;
        double _height = 64;
        //bool _yautorange;
        unsigned int _xprecision = 2;
        unsigned int _yprecision = 2;
        double _xmin = 0;
        double _xmax = 1;
        double _ymin = 0;
        double _ymax = 1;
        double _xtick = 0.5;
        double _ytick = 0.5;
        String _title = "";
        String _xlabel = "";
        String _ylabel = "";
        unsigned int _fcolor = WHITE;
        unsigned int _bcolor = BLACK;

        // Position and size of the graph (minus the labels and title)
        double _graphheight = _height;
        double _graphwidth = _width;
        double _graphxpos = _xpos;
        double _graphypos = _ypos;

        void displayTitle();
        void displayXLabel();
        void displayYLabel();
        void displayXValues();
        void displayYValues();
        void drawGrid();

        // Estimates for the size of different parts of the graph.
        unsigned int titleHeight();
        unsigned int xLabelHeight();
        unsigned int yLabelWidth();
        unsigned int xValueHeight();
        unsigned int yValueWidth();

        double domainToGraphX(double x);
        double domainToGraphY(double y);

        // Last coordinates are used to connect the next data point with a line
        double _haslast = false;
        double _lastx;
        double _lasty;

        char _floatStrBuf[30];
        /* Formatiert eine Float in einen String.
         Achtung: Aus Effizienz- und Komplexitätsgründen wird der String in einem
         statischen Array abgelegt. Daher kann jeweils nur auf die letzte formatierte
         Zahl zugegriffen werden!!
         Argumente:
            - f: Die zu formatierende Zahl
            - precision: Anzahl an Nachkommastellen
         Ausgabe: Zeiger zu dem String mit der formatierten Zahl.
        */
        char *mydtostrf(float f, byte precision);
        char *removelpad(char *str);
        unsigned int textpxwidth(String text, unsigned int fontsize);
        unsigned int textpxheight(String text, unsigned int fontsize);
        unsigned int charpxwidth(unsigned int fontsize);
        unsigned int charpxheight(unsigned int fontsize);
};
