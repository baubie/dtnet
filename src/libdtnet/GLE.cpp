
#include "GLE.h"

using namespace std;
namespace bfs = boost::filesystem;

bool GLE::setPanelProperties(PanelProperties props, PanelID ID) {
    if (ID >= (PanelID)panels.size()) return false;
    panels[ID].properties = props;
    return true;
}
bool GLE::setPanelProperties(PanelProperties props) {
    vector<Panel>::iterator iter;
    for (iter = panels.begin(); iter != panels.end(); ++iter) {
        iter->properties = props;
    }
    return true;
}
GLE::PanelProperties GLE::getPanelProperties(PanelID ID) {
    if (ID >= (PanelID)panels.size()) { PanelProperties r; return r; }
    return panels[ID].properties;
}

bool GLE::verifyData(GLE::Plot &plot)
{
    vector<vector<double> >::iterator iter;
    vector<vector<double> >::iterator iter_eu;
    vector<vector<double> >::iterator iter_ed;
    iter_eu = plot.err_up.begin();
    iter_ed = plot.err_down.begin();
    for (iter = plot.y.begin(); iter != plot.y.end(); ++iter)
    {
        if (iter->size() != plot.x.size())
        {
            cout << "[GLE++] Error: x and y vector sizes do not all match (x: " << plot.x.size() << " y: " << iter->size() << ")." << endl;
            return false;
        }

        if (iter_eu != plot.err_up.end()) {
            if (!iter_eu->empty() && iter_eu->size() != plot.x.size())
            {
                cout << "[GLE++] Error: x and y error_up vector sizes do not all match (x: " << plot.x.size() << " y: " << iter_eu->size() << ")." << endl;
                return false;
            }
            if (!iter_ed->empty() && iter_ed->size() != plot.x.size())
            {
                cout << "[GLE++] Error: x and y error_down vector sizes do not all match (x: " << plot.x.size() << " y: " << iter_ed->size() << ")." << endl;
                return false;
            }
            ++iter_eu;
            ++iter_ed;
        }
    }
    return true;
}

GLE::PanelID GLE::plot(vector< pair<double,double> > &points, PlotProperties properties, GLE::PanelID ID)
{
    Points new_points;
    new_points.points = points;
    new_points.properties = properties;

    Panel panel;
    if (ID == GLE::NEW_PANEL)
    {
        // Get a new ID
        this->panels.push_back(panel);
        ID = this->panels.size() - 1;
    }
    else
    {
        try
        {
            panel = this->panels.at(ID);
        }
        catch (out_of_range outOfRange)
        {
            cout << "[GLE++] Attempted to add to a non-existent panel (" << outOfRange.what() << ")." << endl;
            return false;
        }

    }

    panel.points.push_back(new_points);
    this->panels.at(ID) = panel;
    return ID;
}

GLE::PanelID GLE::plot(vector<double> &x, vector<double> &y, GLE::PlotProperties properties)
{
    return this->plot(x, y, properties, GLE::NEW_PANEL);
}

GLE::PanelID GLE::plot(vector<double> &x, vector<vector<double> > &y, GLE::PlotProperties properties)
{
    vector<vector<double> > empty;
    return this->plot(x, y, empty, empty, properties, GLE::NEW_PANEL);
}

GLE::PanelID GLE::plot(vector<double> &x, vector<vector<double> > &y, vector<vector<double> > &err_up, vector<vector<double> > &err_down, GLE::PlotProperties properties)
{
    return this->plot(x, y, err_up, err_down, properties, GLE::NEW_PANEL);
}

GLE::PanelID GLE::plot(vector<double> &x, vector<double> &y, GLE::PlotProperties properties, GLE::PanelID ID)
{
    vector<vector<double> > tmp;
    tmp.push_back(y);
    vector<vector<double> > empty;
    return this->plot(x, tmp, empty, empty, properties, ID);
}

GLE::PanelID GLE::plot(vector<double> &x, vector<double> &y, vector<double> &err_up, vector<double> &err_down, GLE::PlotProperties properties, GLE::PanelID ID)
{
    vector<vector<double> > tmp;
    vector<vector<double> > tmp_eu;
    vector<vector<double> > tmp_ed;
    tmp.push_back(y);
    tmp_eu.push_back(err_up);
    tmp_ed.push_back(err_down);
    return this->plot(x, tmp, tmp_eu, tmp_ed, properties, ID);
}

GLE::PanelID GLE::plot(vector<double> &x, vector<vector<double> > &y, vector<vector<double> > &err_up, vector<vector<double> > &err_down, GLE::PlotProperties properties, GLE::PanelID ID)
{
    Plot plot;
    plot.x = x;
    plot.properties = properties;

    if (properties.no_y) {
        /** When no_y = true then the signal values are simply x values.
         *  Thus, we must transform each signal into (x,y) points. **/
        plot.properties.zeros = false;
        plot.properties.lineWidth = 0;
        vector<vector<double> > new_y;
        vector<vector<double> >::iterator sigIter;
        vector<double>::iterator valIter;
        vector<double>::iterator tsIter;
        double curY = plot.properties.y_start;

        /** std::find wasn't finding some values for some reason.  Doing manually therefore. **/
        double front = x.front();
        double dt = (x[1] - x[0]);

        for (sigIter = y.begin(); sigIter != y.end(); ++sigIter) {
            vector<double> new_sig(x.size(), 0.0); // Default everything to zero
            for (valIter = sigIter->begin(); valIter != sigIter->end(); ++valIter) {
                int index = (int)((*valIter-front)/dt);
                if (*valIter >= x.front() && *valIter <= x.back()) new_sig[index] = curY;
            }
            curY += plot.properties.y_inc;
            new_y.push_back(new_sig);
        }
        plot.y = new_y;
    } else {
        plot.y = y;
        plot.err_up = err_up;
        plot.err_down = err_down;
    }


    this->verifyData(plot);

    Panel panel;

    if (ID == GLE::NEW_PANEL)
    {
        // Get a new ID
        this->panels.push_back(panel);
        ID = this->panels.size() - 1;
    }
    else
    {
        try
        {
            panel = this->panels.at(ID);
        }
        catch (out_of_range outOfRange)
        {
            cout << "[GLE++] Attempted to add to a non-existent panel (" << outOfRange.what() << ")." << endl;
            return -1;
        }

    }

    panel.plots.push_back(plot);
    this->panels.at(ID) = panel;

    if (properties.no_y) {
        PanelProperties p = this->getPanelProperties(ID);
        p.y_labels = false;
        this->setPanelProperties(p, ID);
    }

    return ID;
}

GLE::PanelID GLE::plot3d(vector<double> &x, vector<double> &y, vector< vector<double> > &z, PlotProperties properties, PanelID ID)
{

    Plot3d plot3d;
    plot3d.x = x;
    plot3d.y = y;
    plot3d.z = z;
    plot3d.properties = properties;

    Panel panel;

    if (ID == GLE::NEW_PANEL)
    {
        // Get a new ID
        this->panels.push_back(panel);
        ID = this->panels.size() - 1;
    }
    else
    {
        try
        {
            panel = this->panels.at(ID);
        }
        catch (out_of_range outOfRange)
        {
            cout << "[GLE++] Attempted to add to a non-existent panel (" << outOfRange.what() << ")." << endl;
            return -1;
        }

    }

    panel.plots3d.push_back(plot3d);
    this->panels.at(ID) = panel;
    return ID;
}

string GLE::getMarker()
{
    string r = *this->iter_marker;
    this->iter_marker++;
    if (this->iter_marker == this->markers.end()) this->iter_marker = this->markers.begin();
    return r;
}

bool GLE::draw()
{
    return this->draw("output.eps");
}

bool GLE::draw(string const &filename)
{
    string gle_script_file = this->gle_script_to_file();

    string type = filename.substr(filename.find('.')+1);
    string basename = filename.substr(0, filename.find('.'));
    if (type == "jpeg") type = "jpg";

    string graphics_dir = type+"_graphics";
    string image_name = graphics_dir + "/" + filename;
    string command;

    if (filename != "_preview_" && filename != "STDOUT" && filename != "_nodraw_" && basename != "temp")
    {
        bfs::create_directory(bfs::path(graphics_dir));
        command = string("gle -d ") + type + string(" -output ") + image_name + " " + gle_script_file;
    }
    else if (basename == "temp")
    {
        command = string("gle -d ") + type + string(" -output temp.") + type + " " + gle_script_file;       
    }
    else if (filename == "_preview_")
    {
        command = string("gle") + string(" -p ")  + gle_script_file;
    }
    else if (filename == "STDOUT")
    {
        command = string("gle -d ") + type + string(" -output ") + image_name + " " + gle_script_file;
    }
    int r = 0;
    if (filename != "_nodraw_")
    {
        r = system(command.c_str());
        if (r != 0) {
            cerr << "[GLE] An error occured." << endl;
            cerr << "[GLE] " << command.c_str() << endl;
        }
        else
        {
            cout << "[GLE] Saved plot to " << filename << endl;
        }
    }

    // Move the temporary files to folder
    vector<Panel>::iterator panel_iter;
    vector<Plot>::iterator plot_iter;
    vector<Points>::iterator points_iter;
    vector<Plot3d>::iterator plot3d_iter;
    if (filename != "_preview_" && filename !="_nodraw_" && filename != "temp")
    {
    string script_dir = "GLE_scripts";
    bfs::create_directory(bfs::path(script_dir));
    bfs::remove_all(bfs::path(script_dir + "/" + basename));
    bfs::create_directory(bfs::path(script_dir + "/" + basename));
    basename = script_dir + "/" + basename;
    string scriptName = basename + "/script.gle";
    string dataName;
    bfs::copy_file(bfs::path(gle_script_file), bfs::path(scriptName));
    bfs::remove(bfs::path(gle_script_file));
    for( panel_iter = this->panels.begin(); panel_iter != this->panels.end(); ++panel_iter)
    {
        for ( plot_iter = panel_iter->plots.begin(); plot_iter != panel_iter->plots.end(); ++plot_iter)
        {
            dataName = basename + "/" + plot_iter->data_file.substr(plot_iter->data_file.find_last_of("/")+1);
            bfs::copy_file(bfs::path(plot_iter->data_file), bfs::path(dataName));
            bfs::remove(bfs::path(plot_iter->data_file));
        }
        for ( points_iter = panel_iter->points.begin(); points_iter != panel_iter->points.end(); ++points_iter )
        {
            dataName = basename + "/" + points_iter->data_file.substr(points_iter->data_file.find_last_of("/")+1);
            bfs::copy_file(bfs::path(points_iter->data_file), bfs::path(dataName));
            bfs::remove(bfs::path(points_iter->data_file));
        }
        for ( plot3d_iter = panel_iter->plots3d.begin(); plot3d_iter != panel_iter->plots3d.end(); ++plot3d_iter )
        {
            dataName = basename + "/" + plot3d_iter->data_file.substr(plot3d_iter->data_file.find_last_of("/")+1);
            bfs::copy_file(bfs::path(plot3d_iter->data_file), bfs::path(dataName));
            bfs::remove(bfs::path(plot3d_iter->data_file));
        }
    }
    } else {
	// Just previewing so remove temporary files
	    bfs::remove(bfs::path(gle_script_file));
	    for( panel_iter = this->panels.begin(); panel_iter != this->panels.end(); ++panel_iter)
	    {
		for ( plot_iter = panel_iter->plots.begin(); plot_iter != panel_iter->plots.end(); ++plot_iter)
		{
		    bfs::remove(bfs::path(plot_iter->data_file));
		}
		for ( points_iter = panel_iter->points.begin(); points_iter != panel_iter->points.end(); ++points_iter )
		{
		    bfs::remove(bfs::path(points_iter->data_file));
		}
		for ( plot3d_iter = panel_iter->plots3d.begin(); plot3d_iter != panel_iter->plots3d.end(); ++plot3d_iter )
		{
		    bfs::remove(bfs::path(plot3d_iter->data_file));
		}
	    }
    }

    return (r == 0);
}

bool GLE::data_to_file()
{

    vector<Panel>::iterator panel_iter;
    vector<Plot>::iterator plot_iter;
    vector<double>::iterator x_iter;
    vector<double>::iterator y_iter;
    vector<Points>::iterator points_iter;
    vector<Plot3d>::iterator plot3d_iter;
    vector< pair<double,double> >::iterator pair_iter;
    vector<vector<double> >::iterator all_y_iter;
    map<double, vector<double> > y;
    vector<double>::iterator values_y_iter;
    map<double, vector<double> >::iterator values_iter;

    for( panel_iter = this->panels.begin(); panel_iter != this->panels.end(); ++panel_iter)
    { /**< Loop over each panel. */

        for ( plot3d_iter = panel_iter->plots3d.begin(); plot3d_iter != panel_iter->plots3d.end(); ++plot3d_iter )
        { /**< Loop over each set of 3D data (probably only one, but this is general). **/
            char data_filename[] = "gle_data_XXXXXX";
            int pTemp = mkstemp(data_filename);
            boost::iostreams::file_descriptor_sink sink( pTemp, boost::iostreams::close_handle );
            boost::iostreams::stream<boost::iostreams::file_descriptor_sink> of( sink );
            if (!of)
            {
               cerr << "[GLE] Unable to create temporary file." << endl;
               return false;
            }
            plot3d_iter->data_file = string(data_filename);
            plot3d_iter->data_file += ".z"; // GLE requires the .z extension to work
            of << "! nx " << plot3d_iter->x.size() << " ny " << plot3d_iter->y.size();
            of << " xmin " << plot3d_iter->x.front() << " xmax " << plot3d_iter->x.back();
            of << " ymin " << plot3d_iter->y.front() << " ymax " << plot3d_iter->y.back();
            of << endl;
            for (vector< vector<double> >::iterator x = plot3d_iter->z.begin(); x != plot3d_iter->z.end(); ++x) {
                for (vector<double>::iterator y = x->begin(); y != x->end(); ++y) {
                    of << fixed << setprecision(3) << *y << " ";
                    if (*y > plot3d_iter->z_max) plot3d_iter->z_max = *y;
                    if (*y < plot3d_iter->z_min) plot3d_iter->z_min = *y;
                }
                of << endl;
            }
            close ( pTemp );
            bfs::copy_file( bfs::path(data_filename), bfs::path(plot3d_iter->data_file) );
            bfs::remove( bfs::path(data_filename) );
        }

        for ( points_iter = panel_iter->points.begin(); points_iter != panel_iter->points.end(); ++points_iter )
        { /**< Loop over each set of points. */
            char data_filename[] = "gle_data_XXXXXX";
            int pTemp = mkstemp(data_filename);
            boost::iostreams::file_descriptor_sink sink( pTemp, boost::iostreams::close_handle );
            boost::iostreams::stream<boost::iostreams::file_descriptor_sink> of( sink );
            if (!of)
            {
               cerr << "[GLE] Unable to create temporary file." << endl;
               return false;
            }
            points_iter->data_file = string(data_filename);
            for (pair_iter = points_iter->points.begin(); pair_iter != points_iter->points.end(); ++pair_iter) {
                        of <<  fixed << setprecision(3) << pair_iter->first << "," << pair_iter->second << endl;
            }
            close ( pTemp );
        }

        // Error bar iterators
        vector<vector<double> >::iterator iter_aeu;
        vector<vector<double> >::iterator iter_aed;
        vector<double>::iterator iter_eu;
        vector<double>::iterator iter_ed;
        bool error_bars;

        for ( plot_iter = panel_iter->plots.begin(); plot_iter != panel_iter->plots.end(); ++plot_iter)
        { /**< Loop over each plot in this panel. */

            error_bars = false;
            iter_aeu = plot_iter->err_up.begin();
            iter_aed = plot_iter->err_down.begin();
            if (iter_aeu != plot_iter->err_up.end()) error_bars = true;

            for ( all_y_iter = plot_iter->y.begin(); all_y_iter != plot_iter->y.end(); ++all_y_iter)
            { /**< Loop over each trace in this plot. */
                x_iter = plot_iter->x.begin(); // We assume x and y are the same size since verifyData() returned true.
                if (error_bars) {
                    iter_eu = iter_aeu->begin();
                    iter_ed = iter_aed->begin();
                }
                for ( y_iter = all_y_iter->begin(); y_iter != all_y_iter->end(); ++y_iter)
                { /**< Loop over each y value in this trace. */
                    y[*x_iter].push_back(*y_iter);
                    if (error_bars) {
                        y[*x_iter].push_back(*iter_eu);
                        y[*x_iter].push_back(*iter_ed);
                        ++iter_eu;
                        ++iter_ed;
                    }
                    ++x_iter;
                }
            }

            char data_filename[] = "gle_data_XXXXXX";
            int pTemp = mkstemp(data_filename);
            boost::iostreams::file_descriptor_sink sink( pTemp, boost::iostreams::close_handle );
            boost::iostreams::stream<boost::iostreams::file_descriptor_sink> of( sink );
            if (!of)
            {
               cerr << "[GLE] Unable to create temporary file." << endl;
               return false;
            }
            plot_iter->data_file = string(data_filename);
            for (values_iter = y.begin(); values_iter != y.end(); ++values_iter) {
                of << fixed << setprecision(3)  << values_iter->first;
                for ( values_y_iter = values_iter->second.begin(); values_y_iter != values_iter->second.end(); ++values_y_iter ) {
                    if (plot_iter->properties.zeros || *values_y_iter != 0) {
                        of << fixed << setprecision(3) << "," << *values_y_iter;
                    } else {
                        of <<  "," << "*"; // Skip this value
                    }
                }
                of << endl;
            }
            close ( pTemp );
            y.clear();
        }
    }
    return true;
}

string GLE::gle_script_to_file()
{
    this->data_to_file();

    char filename[] = "gle_script_XXXXXX";
    int pTemp = mkstemp(filename);
    boost::iostreams::file_descriptor_sink sink( pTemp, boost::iostreams::close_handle );
    boost::iostreams::stream<boost::iostreams::file_descriptor_sink> out( sink );

    vector<Panel>::iterator panel_iter;
    vector<Plot>::iterator plot_iter;
    vector<Plot3d>::iterator plot3d_iter;
    vector<Points>::iterator points_iter;
    vector< pair<double,double> >::iterator pair_iter;
    vector<vector<double> >::iterator y_iter;
    Color color;
    Color diff;

    out << "!!!!!!!!!!!!!!!!!!!!!!" << endl;
    out << "! Generated by dtnet !" << endl;
    out << "!!!!!!!!!!!!!!!!!!!!!!" << endl << endl;
    out << "size " << this->canvasProperties.width << " " << this->canvasProperties.height << endl;
    out << "set font psh" << endl;
    out << "set hei 0.3000" << endl;

    if (this->canvasProperties.auto_layout)
    {
        float panel_width, panel_height;
        int rows;
        int count = 0;
        int plot_num = 0;
        int last_row_count = 0;
        rows = ceil( (float)this->panels.size() / (float)this->canvasProperties.columns );
        panel_width = (float)( (this->canvasProperties.width - 1.05*this->canvasProperties.margin_left*(this->canvasProperties.columns-1)) / (float)this->canvasProperties.columns );
        panel_height = (float)( (this->canvasProperties.height - 1.25*this->canvasProperties.margin_top*(rows-1)) / (float)rows );

        panel_iter = this->panels.begin();
        for ( int r = 1; r <= rows; ++r )
        {
            for ( int c = 1; c <= this->canvasProperties.columns; ++c )
            {
                last_row_count = c;
                ++count;
                out << endl;
                out << "!!!!!!!!!!!" << endl;
                out << "! PANEL " << count << " !" << endl;
                out << "!!!!!!!!!!!" << endl;

                out << "include \"color.gle\"" << endl;

                out << "begin object graph" << count << endl;

                if (panel_iter == --(this->panels.end()) && this->panels.size() > 1) panel_height *= 1.2;

                if (panel_iter->plots3d.size() > 0) {
                    if (panel_iter->plots3d[0].properties.usemap == true) {
                        // Output 3D data using a heatmap type display
                        out << "begin graph" << endl;
                        out << "xaxis min " << panel_iter->plots3d[0].x.front() << " max " << panel_iter->plots3d[0].x.back() << endl;
                        out << "yaxis min " << panel_iter->plots3d[0].y.front() << " max " << panel_iter->plots3d[0].y.back() << endl;
                        out << "size " << (panel_width-1) << " " << panel_height << endl;
                        out << "scale auto" << endl;
                        out << "xtitle \"" << panel_iter->properties.x_title << "\"" << endl;
                        out << "ytitle \"" << panel_iter->properties.y_title << "\"" << endl;
                        out << "title \"" << panel_iter->properties.title << "\"" << endl;
                        out << "colormap \"" << panel_iter->plots3d[0].data_file << "\"";
                        out << " " << panel_iter->plots3d[0].x.size() << " " << panel_iter->plots3d[0].y.size();
                        out << " zmin " << panel_iter->plots3d[0].z_min << " zmax " << panel_iter->plots3d[0].z_max;
                        out << endl;
                        out << "end graph" << endl;
                        out << "amove xg(xgmax)+0.3 yg(ygmin)" << endl;
                        out << "color_range_vertical " << panel_iter->plots3d[0].z_min << " " << panel_iter->plots3d[0].z_max;
                        out << " 0.5 palette gray pixels 500 format \"fix 1\"" << endl;
                    } else {
                        // Output 3D data using a 3D graph
                        out << "begin surface" << endl;
                        out << "xaxis min " << panel_iter->plots3d[0].x.front() << " max " << panel_iter->plots3d[0].x.back() << endl;
                        out << "yaxis min " << panel_iter->plots3d[0].y.front() << " max " << panel_iter->plots3d[0].y.back() << endl;
                        out << "size " << panel_width << " " << panel_height << endl;
                        out << "xtitle \"" << panel_iter->properties.x_title << "\"" << endl;
                        out << "ytitle \"" << panel_iter->properties.y_title << "\"" << endl;
                        out << "title \"" << panel_iter->properties.title << "\"" << endl;
                        out << "data \"" << panel_iter->plots3d[0].data_file << "\"" << endl;
                        out << "end surface" << endl;

                    }
                } else {
                out << "begin graph" << endl;
                out << "nobox" << endl;
                out << "x2axis off" << endl;
                out << "y2axis off" << endl;
                out << "size " << panel_width << " " << panel_height << endl;
                out << "scale auto" << endl;
                out << "xtitle \"" << panel_iter->properties.x_title << "\"" << endl;
                out << "ytitle \"" << panel_iter->properties.y_title << "\"" << endl;
                out << "xticks length -0.1" << endl;
                out << "yticks length -0.1" << endl;
                out << "title \"" << panel_iter->properties.title << "\"" << endl;
                out << "xaxis min " << panel_iter->plots[0].x.front() << " max " << panel_iter->plots[0].x.back() << endl;

                if (panel_iter->properties.y_min != GLE::UNDEFINED || panel_iter->properties.y_max != GLE::UNDEFINED) {
                    out << "yaxis ";
                    if (panel_iter->properties.y_min != GLE::UNDEFINED) out << "min " << panel_iter->properties.y_min << " ";
                    if (panel_iter->properties.y_max != GLE::UNDEFINED) out << "max " << panel_iter->properties.y_max << " ";
                    out << endl;
                }

                if (panel_iter->properties.x_min != GLE::UNDEFINED || panel_iter->properties.x_max != GLE::UNDEFINED) {
                    out << "xaxis ";
                    if (panel_iter->properties.x_min != GLE::UNDEFINED) out << "min " << panel_iter->properties.x_min << " ";
                    if (panel_iter->properties.x_max != GLE::UNDEFINED) out << "max " << panel_iter->properties.x_max << " ";
                    out << endl;
                }

                if (panel_iter->properties.y_nticks != GLE::UNDEFINED) {
                    out << "yaxis nticks " << panel_iter->properties.y_nticks << endl;
                }

                if (panel_iter->properties.x_labels == false) {
                    out << "xaxis off" << endl;
                }
                if (panel_iter->properties.y_labels == false) {
                    out << "yaxis off" << endl;
                }
                if (panel_iter->properties.x_dsubticks != GLE::UNDEFINED) {
                    out << "xaxis dsubticks " << panel_iter->properties.x_dsubticks << endl;
                }
                if (panel_iter->properties.x_dticks != GLE::UNDEFINED) {
                    out << "xaxis dticks " << panel_iter->properties.x_dticks << endl;
                }
                if (panel_iter->properties.y_dsubticks != GLE::UNDEFINED) {
                    out << "yaxis dsubticks " << panel_iter->properties.y_dsubticks << endl;
                }
                if (panel_iter->properties.y_dticks != GLE::UNDEFINED) {
                    out << "yaxis dticks " << panel_iter->properties.y_dticks << endl;
                }
                if (panel_iter->properties.x_labels_hei != GLE::UNDEFINED) {
                    out << "xlabels hei " << panel_iter->properties.x_labels_hei << endl;
                }
                if (panel_iter->properties.y_labels_hei != GLE::UNDEFINED) {
                    out << "ylabels hei " << panel_iter->properties.y_labels_hei << endl;
                }
                if (panel_iter->properties.x_labels_dist != GLE::UNDEFINED) {
                    out << "xlabels dist " << panel_iter->properties.x_labels_dist << endl;
                }
                if (panel_iter->properties.y_labels_dist != GLE::UNDEFINED) {
                    out << "ylabels dist " << panel_iter->properties.y_labels_dist << endl;
                }

                plot_num = 1;
                for ( plot_iter = panel_iter->plots.begin(); plot_iter != panel_iter->plots.end(); ++plot_iter)
                {
                    out << "data \"" << plot_iter->data_file << "\"" << endl;
                    diff.r = (plot_iter->properties.last.r - plot_iter->properties.first.r) / plot_iter->y.size();
                    diff.g = (plot_iter->properties.last.g - plot_iter->properties.first.g) / plot_iter->y.size();
                    diff.b = (plot_iter->properties.last.b - plot_iter->properties.first.b) / plot_iter->y.size();
                    color = plot_iter->properties.first;

                    for ( y_iter = plot_iter->y.begin(); y_iter != plot_iter->y.end(); ++y_iter)
                    {
                        if (plot_iter->properties.lineWidth > 0) {
                            out << "d" << plot_num << " line color CVTRGB(" << color.r << "," << color.g << "," << color.b << ")" << " lwidth " << plot_iter->properties.lineWidth << endl;
                        }

                        string marker;
                        if (plot_iter->properties.pointSize > 0) {
                            marker = plot_iter->properties.marker;
                            if (marker == "__series__") marker = this->getMarker();
                            out << "d" << plot_num << " marker " << marker << " msize " << plot_iter->properties.pointSize;
                            if (panel_iter->properties.legend && plot_iter->properties.inlegend) {
                                out << " key  \"Legend Text\"";
                            } else {
                                out << " !key  \"Legend Text\"";
                            }
                            out << endl;
                            if (plot_iter->properties.lineWidth > 0) {
                                out << "d" << plot_num << " color CVTRGB(" << color.r << "," << color.g << "," << color.b << ")" << endl;
                            }
                        }

                        if (plot_iter->properties.nomiss) {
                            out << "d" << plot_num << " nomiss" << endl;
                        }

                        if (!plot_iter->err_up.empty()) {
                            out << "d" << plot_num << " errup d" << (plot_num+1) << " errdown d" << (plot_num+2) << endl;
                            plot_num += 2;
                        }

                        color.r += diff.r;
                        color.g += diff.g;
                        color.b += diff.b;
                        ++plot_num;
                    }
                }
                for ( points_iter = panel_iter->points.begin(); points_iter != panel_iter->points.end(); ++points_iter )
                {
                    out << "data \"" << points_iter->data_file << "\"" << endl;
                    out << "d" << plot_num << " marker dot msize " << points_iter->properties.pointSize << endl;
                    ++plot_num;
                }
                if (panel_iter->properties.legend ) {
                    out << "key compact" << endl;
                    out << "key nobox" << endl;
                }
                out << "end graph" << endl;
                }
                out << "end object" << endl;

                ++panel_iter;
                if (panel_iter == this->panels.end()) { c = this->canvasProperties.columns + 1; r = rows + 1; }
            }
        }

        // Now display them backwards (bottom->top, left->right)
        stringstream str_y("0.1");
        stringstream str_x("0");
        ++count;
        for (int r = rows; r >= 1; --r)
        {
            str_x.str("0.1");
            for (int c = 1; c <= this->canvasProperties.columns; c++)
            {
                --count;
                out << "amove " << str_x.str()
                                << " "
                                << str_y.str()
                                << endl;
                out << "draw graph" << count << ".bl" << endl;

                str_x.str("");
                str_x << "ptx(graph" << count << ".br)+" << this->canvasProperties.margin_left;

                if (r == rows && c == last_row_count) { c = this->canvasProperties.columns + 1; }
            }
            str_y.str("");
            str_y << "pty(graph" << count << ".tc)+" << this->canvasProperties.margin_top;
        }


    }

    close ( pTemp );
    string old_filename = string(filename);
    string new_filename = string(filename) + ".gle";
    bfs::copy_file(bfs::path(old_filename), bfs::path(new_filename));
    bfs::remove(bfs::path(old_filename));
    return new_filename;
}

