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
#include <algorithm>
#include <sys/stat.h>
#include <sys/types.h>

#include <boost/filesystem.hpp>
#include <boost/iostreams/device/file_descriptor.hpp>
#include <boost/iostreams/stream.hpp>

#include <math.h>
#include "debug.h"

class GLE
{
    public:

        typedef int PanelID;
        static const int NEW_PANEL = -1;
        static const double UNDEFINED = -91348434;

        std::string *buffer;

        std::vector<std::string> markers;
        std::string getMarker();
        std::vector<std::string>::iterator iter_marker;

        GLE() {
            this->markers.push_back("fcircle");
            this->markers.push_back("wsquare");
            this->markers.push_back("ftriangle");
            this->markers.push_back("wdiamond");
            this->markers.push_back("wcircle");
            this->markers.push_back("fsquare");
            this->markers.push_back("wtriangle");
            this->markers.push_back("fdiamond");
            this->iter_marker = this->markers.begin();
        }

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
            std::string marker;
            bool zeros;
            bool nomiss;
            bool no_y;
            bool usemap;
            bool inlegend;

            /** For use when no_y = true **/
            double y_start;
            double y_inc;

            Color first;
            Color last;

            PlotProperties() :
                lineWidth(0.010),
                pointSize(0.1),
                marker("__series__"),
                zeros(true),
                nomiss(true),
                no_y(false),
                usemap(false),
                inlegend(true),
                y_start(1),
                y_inc(1)
            {}
        };

        struct PanelProperties {
            bool box;
            bool legend;
            std::string title;
            std::string x_title;
            std::string y_title;
            std::string z_title;
            bool y_labels;
            bool x_labels;
            double x_min;
            double x_max;
            double y_min;
            double y_max;
            int y_nticks;
            double x_dsubticks;
            double x_dticks;
            double y_dsubticks;
            double y_dticks;
            double x_labels_hei;
            double y_labels_hei;
            double x_labels_dist;
            double y_labels_dist;

            // Only used when the canvas has autoplacement turned off
            // Not yet implemented
            int pos_x;
            int pos_y;
            int width;
            int height;

            PanelProperties() :
                box(true),
                legend(false),
                title("Some Plot"),
                x_title("x"),
                y_title("y"),
                z_title("z"),
                y_labels(true),
                x_labels(true),
                x_min(UNDEFINED),
                x_max(UNDEFINED),
                y_min(UNDEFINED),
                y_max(UNDEFINED),
                y_nticks(UNDEFINED),
                x_dsubticks(UNDEFINED),
                x_dticks(UNDEFINED),
                y_dsubticks(UNDEFINED),
                y_dticks(UNDEFINED),
                x_labels_hei(UNDEFINED),
                y_labels_hei(UNDEFINED),
                x_labels_dist(UNDEFINED),
                y_labels_dist(UNDEFINED)
            {}
        };

        CanvasProperties canvasProperties;

        // Various different plot functions

        //This is the main one that other plot()'s call
        PanelID plot(std::vector<double> &x, std::vector<std::vector<double> > &y, std::vector<std::vector<double> > &err_up, std::vector<std::vector<double> > &err_down, PlotProperties properties, PanelID panel); // Plot a Multiple x-y curves with the same x by adding it to an existing panel



        PanelID plot(std::vector<double> &x, std::vector<double> &y, PlotProperties properties); // Plot a single x-y curve
        PanelID plot(std::vector< std::pair<double,double> > &points, PlotProperties properties, PanelID ID);  // Plot a set of points
        PanelID plot(std::vector<double> &x, std::vector<std::vector<double> > &y, PlotProperties properties); // Plot a Multiple x-y curves with the same x
        PanelID plot(std::vector<double> &x, std::vector<std::vector<double> > &y, std::vector<std::vector<double> > &err_up, std::vector<std::vector<double> > &err_down, PlotProperties properties); // Plot a Multiple x-y curves with the same x
        PanelID plot(std::vector<double> &x, std::vector<double> &y, PlotProperties properties, PanelID panel); // Plot a single x-y curve by adding it to an existing panel
        PanelID plot(std::vector<double> &x, std::vector<double> &y, std::vector<double> &err_up, std::vector<double> &err_down, PlotProperties properties, PanelID panel);

        PanelID plot3d(std::vector<double> &x, std::vector<double> &y, std::vector< std::vector<double> > &z, PlotProperties properties, PanelID ID);

        bool setPanelProperties(PanelProperties properties, PanelID ID); // Set the panel properties for a particular panel
        bool setPanelProperties(PanelProperties properties); // Set the panel properties for all panels

        PanelProperties getPanelProperties(PanelID ID);

        bool draw(std::string const &filename); // Draw the plot to a filename
        bool draw(); // Draw a plot to the default filename

    private:

        struct Plot {
            std::vector<std::vector<double> > y;
            std::vector<std::vector<double> > err_up;
            std::vector<std::vector<double> > err_down;
            std::vector<double> x;
            PlotProperties properties;
            std::string data_file;
        };

        struct Plot3d {
            std::vector<double> y;
            std::vector<double> x;
            std::vector< std::vector<double> > z;
            double z_min, z_max;
            PlotProperties properties;
            std::string data_file;
        };

        struct Points {
            std::vector< std::pair<double,double> > points;
            PlotProperties properties;
            std::string data_file;
        };

        struct Panel {
            PanelProperties properties;
            std::vector<Plot> plots;
            std::vector<Points> points;
            std::vector<Plot3d> plots3d;
        };

        std::vector<Panel> panels;

        bool verifyData(Plot &plot);
        bool data_to_file();
        std::string gle_script_to_file();
};

#endif
