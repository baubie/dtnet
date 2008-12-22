/** GLE++
 *  GLE interface for C++
 *  Written by Brandon Aubie
 */

#ifndef GLE_H
#define GLE_H

#include <vector>
#include <map>
#include <cstdio>
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <iomanip>
#include <boost/iostreams/device/file_descriptor.hpp>
#include <boost/iostreams/stream.hpp>
#include <math.h>

class GLE
{
    public:
        
        typedef int PanelID;
        typedef int StyleID;

        static const int STYLE_LINE = 100;
        static const int STYLE_POINT = 101;
        static const int NEW_PANEL = -1;

        struct Color {
            float r;
            float g;
            float b;

            Color() :
                r(0.0),
                g(0.0),
                b(0.0)
            {}
        };

        struct CanvasProperties {
            float width;
            float height;
            int columns;
            float margin_top;
            float margin_left;
            bool auto_layout;

            CanvasProperties() :
                width(8.5),
                height(8.5),
                columns(1),
                margin_top(0.50),
                margin_left(0.25),
                auto_layout(true)
            {}
        };

        struct PlotProperties {
            float lineWidth;
            float pointSize;
            StyleID style;
            Color first;
            Color last;

            PlotProperties() :
                lineWidth(4),
                pointSize(5),
                style(STYLE_LINE)
            {}
        };

        struct PanelProperties {
            bool box;
            std::string title;
            std::string x_title;
            std::string y_title;
            std::string z_title;

            // Only used when the canvas has autoplacement turned off
            int pos_x;
            int pos_y;
            int width;
            int height;

            PanelProperties() :
                box(true),
                title("Some Plot"),
                x_title("x"),
                y_title("y"),
                z_title("z")
            {}
        };
        
        CanvasProperties canvasProperties;

        // Various different plot functions
        PanelID plot(std::vector<double> const &x, std::vector<double> const &y, PlotProperties properties); // Plot a single x-y curve
        PanelID plot(std::vector<double> const &x, std::vector<std::vector<double> > const &y, PlotProperties properties); // Plot a Multiple x-y curves with the same x
        PanelID plot(std::vector<double> const &x, std::vector<double> const &y, PlotProperties properties, PanelID panel); // Plot a single x-y curve by adding it to an existing panel
        PanelID plot(std::vector<double> const &x, std::vector<std::vector<double> > const &y, PlotProperties properties, PanelID panel); // Plot a Multiple x-y curves with the same x by adding it to an existing panel

        bool setPanelProperties(PanelProperties properties, PanelID ID); // Set the panel properties for a particular panel
        bool setPanelProperties(PanelProperties properties); // Set the panel properties for all panels

        bool draw(std::string const &filename); // Draw the plot to a filename
        bool draw(); // Draw a plot to the default filename

    private:

        struct Plot {
            std::vector<std::vector<double> > y;
            std::vector<double> x;
            PlotProperties properties;
            std::string data_file;
        };

        struct Panel {
            PanelProperties properties;
            std::vector<Plot> plots;
        };

        std::vector<Panel> panels;

        bool verifyData(Plot &plot);
        bool data_to_file();
        std::string gle_script_to_file();
};

#endif
