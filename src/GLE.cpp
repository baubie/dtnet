
#include "GLE.h"

using namespace std;


bool GLE::setPanelProperties(PanelProperties props, PanelID ID) {
    if (ID >= panels.size()) return false;
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
    if (ID >= panels.size()) { PanelProperties r; return r; }
    return panels[ID].properties;
}

bool GLE::verifyData(GLE::Plot &plot)
{
    vector<vector<double> >::iterator iter;
    for (iter = plot.y.begin(); iter != plot.y.end(); ++iter)
    {
        if (iter->size() != plot.x.size())
        {
            cout << "[GLE++] Error: x and y vector sizes do not all match (x: " << plot.x.size() << " y: " << iter->size() << ")." << endl;
            return false;
        }
    }
    return true;
}

GLE::PanelID GLE::plot(vector<double> &x, vector<double> &y, GLE::PlotProperties properties)
{
    return this->plot(x, y, properties, GLE::NEW_PANEL);
}

GLE::PanelID GLE::plot(vector<double> &x, vector<vector<double> > &y, GLE::PlotProperties properties)
{
    return this->plot(x, y, properties, GLE::NEW_PANEL);
}

GLE::PanelID GLE::plot(vector<double> &x, vector<double> &y, GLE::PlotProperties properties, GLE::PanelID ID)
{
    vector<vector<double> > tmp;
    tmp.push_back(y);
    return this->plot(x, tmp, properties, ID);
}

GLE::PanelID GLE::plot(vector<double> &x, vector<vector<double> > &y, GLE::PlotProperties properties, GLE::PanelID ID)
{
    GLE::Plot plot;
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
        int tsIndex;

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
            return false;
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


bool GLE::draw()
{
    return this->draw("output.eps");
}

bool GLE::draw(string const &filename)
{
    string gle_script_file = this->gle_script_to_file();
    cout << "[GLE] Saving data to temporary file...";
    string command = string("gle -output ") + filename + " " + gle_script_file;
    cout << "DONE" << endl;

    int r = system(command.c_str());
	if (r != 0) {
		cout << "[GLE] An error occured." << endl;
    }
	else
    {
	    cout << "[GLE] Saved plot to " << filename << endl;
        if (GLE::gv) {
            string command = string("gv ") + filename + " &"; // Try to run ghostview in the background.
            int r = system(command.c_str());
            if (r != 0) {
                cout << "[GLE] Ghostview preview is unavailable." << endl;
            }
        }
    }

    // Delete the temporary files
    vector<Panel>::iterator panel_iter;
    vector<Plot>::iterator plot_iter;
    remove(gle_script_file.c_str());
    for( panel_iter = this->panels.begin(); panel_iter != this->panels.end(); ++panel_iter) 
    {
        for ( plot_iter = panel_iter->plots.begin(); plot_iter != panel_iter->plots.end(); ++plot_iter)
        {
            remove(plot_iter->data_file.c_str());
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
    vector<vector<double> >::iterator all_y_iter;
    map<double, vector<double> > y;
    vector<double>::iterator values_y_iter;
    map<double, vector<double> >::iterator values_iter;

    for( panel_iter = this->panels.begin(); panel_iter != this->panels.end(); ++panel_iter) 
    { /**< Loop over each panel. */
        for ( plot_iter = panel_iter->plots.begin(); plot_iter != panel_iter->plots.end(); ++plot_iter)
        { /**< Loop over each plot in this panel. */
            for ( all_y_iter = plot_iter->y.begin(); all_y_iter != plot_iter->y.end(); ++all_y_iter)
            { /**< Loop over each trace in this plot. */
                x_iter = plot_iter->x.begin(); // We assume x and y are the same size since verifyData() returned true.
                for ( y_iter = all_y_iter->begin(); y_iter != all_y_iter->end(); ++y_iter)
                { /**< Loop over each y value in this trace. */
                    y[*x_iter].push_back(*y_iter);
                    ++x_iter;
                }
            }    

            char data_filename[] = "/tmp/gle_data_XXXXXX";
            int pTemp = mkstemp(data_filename);
            boost::iostreams::file_descriptor_sink sink( pTemp );
            boost::iostreams::stream<boost::iostreams::file_descriptor_sink> of( sink );
            if (!of) 
            {
               cout << "[GLE] Unable to create temporary file." << endl;
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

    char filename[] = "/tmp/gle_script_XXXXXX";
    int pTemp = mkstemp(filename);
    boost::iostreams::file_descriptor_sink sink( pTemp );
    boost::iostreams::stream<boost::iostreams::file_descriptor_sink> out( sink );

    vector<Panel>::iterator panel_iter;
    vector<Plot>::iterator plot_iter;
    vector<vector<double> >::iterator y_iter;
    Color color;
    Color diff;

    out << "!!!!!!!!!!!!!!!!!!!!!!" << endl;
    out << "! Generated by dtnet !" << endl;
    out << "!!!!!!!!!!!!!!!!!!!!!!" << endl << endl;
    out << "size " << this->canvasProperties.width << " " << this->canvasProperties.height << endl;

    if (this->canvasProperties.auto_layout)
    {
        float panel_width, panel_height;
        int rows;
        int count = 0;
        int plot_num = 0;
        int last_row_count = 0;
        rows = ceil( (float)this->panels.size() / (float)this->canvasProperties.columns );
        panel_width = (float)( (this->canvasProperties.width - 1.05*this->canvasProperties.margin_left*(this->canvasProperties.columns-1)) / (float)this->canvasProperties.columns );
        panel_height = (float)( (this->canvasProperties.height - 1.05*this->canvasProperties.margin_top*(rows-1)) / (float)rows );

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


                out << "begin object graph" << count << endl;
                out << "begin graph" << endl;
                out << "size " << panel_width << " " << panel_height << endl;
                out << "scale auto" << endl;
                out << "xtitle \"" << panel_iter->properties.x_title << "\"" << endl;
                out << "ytitle \"" << panel_iter->properties.y_title << "\"" << endl;
                out << "title \"" << panel_iter->properties.title << "\"" << endl;
                out << "xaxis min " << panel_iter->plots[0].x.front() << " max " << panel_iter->plots[0].x.back() << endl;

                if (panel_iter->properties.y_min != GLE::UNDEFINED || panel_iter->properties.y_max != GLE::UNDEFINED) {
                    out << "yaxis ";
                    if (panel_iter->properties.y_min != GLE::UNDEFINED) out << "min " << panel_iter->properties.y_min << " ";
                    if (panel_iter->properties.y_max != GLE::UNDEFINED) out << "max " << panel_iter->properties.y_max << " ";
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

                for ( plot_iter = panel_iter->plots.begin(); plot_iter != panel_iter->plots.end(); ++plot_iter)
                {
                    out << "data \"" << plot_iter->data_file << "\"" << endl;
                    diff.r = (plot_iter->properties.last.r - plot_iter->properties.first.r) / plot_iter->y.size();
                    diff.g = (plot_iter->properties.last.g - plot_iter->properties.first.g) / plot_iter->y.size();
                    diff.b = (plot_iter->properties.last.b - plot_iter->properties.first.b) / plot_iter->y.size();
                    color = plot_iter->properties.first;
                    
                    plot_num = 1;
                    for ( y_iter = plot_iter->y.begin(); y_iter != plot_iter->y.end(); ++y_iter)
                    {
                        out << "d" << plot_num << " line color CVTRGB(" << color.r << "," << color.g << "," << color.b << ")" << " lwidth " << plot_iter->properties.lineWidth << endl;
                        if (plot_iter->properties.pointSize > 0) out << "d" << plot_num << " marker " << plot_iter->properties.shape << " msize " << plot_iter->properties.pointSize << endl; 
                        if (plot_iter->properties.nomiss) out << "d" << plot_num << " nomiss" << endl;
                        color.r += diff.r;
                        color.g += diff.g;
                        color.b += diff.b;
                        ++plot_num;
                    }
                }
                out << "end graph" << endl;
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
    rename(old_filename.c_str(), new_filename.c_str());
    return new_filename; 
}

