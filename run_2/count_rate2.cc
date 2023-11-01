/*
All the informations and plots about the count rate are generated in this file.
*/

#include <TFile.h>
#include <TTree.h>
#include <string.h>
#include <chrono>
#include <cmath>
#include <TH2D.h>

std::vector<Double_t> getStats(std::vector<Double_t> &dt);

// calculates delta time

void count_rate2(std::string path, std::string particle, Double_t energy_min, std::string draw_opt, bool draw)
{
    auto start = std::chrono::system_clock::now();
    // constants
    Double_t psPerEvent = 1e4; // 10^9 particles/s in ps
    std::string str_energy = std::to_string(energy_min);

    //variables
    TH1I *hist_particles = new TH1I();
    hist_particles->SetNameTitle("hist_particles", (std::string("particle overview ") + str_energy + std::string(" eV")).c_str());
    hist_particles->SetBinsLength(6);

    // root file and trees
    TFile *file = new TFile(path.c_str(), "read");
    TTree *hits = (TTree *)file->Get("hits");

    // needed variables from the tree
    char particle_name[128];
    Int_t event_number;
    Int_t det_id, strip_id;
    Double_t time, edep;

    // connect those variables to the tree
    hits->SetBranchAddress("name", &particle_name);
    hits->SetBranchAddress("event", &event_number);
    hits->SetBranchAddress("Det_ID", &det_id);
    hits->SetBranchAddress("Strip_ID", &strip_id);
    hits->SetBranchAddress("Hit_time", &time);
    hits->SetBranchAddress("edep", &edep);

    // timestamps[detector][strip][entry] as matrix
    // Needed when looping only once through all entries from root file
    std::vector<std::vector<std::vector<Double_t> > > timestamps;

    // create correct dimension (8 detectors x 1024 strips)
    timestamps.resize(8); // 8 detectors
    for (int i = 0; i < 8; i++)
    {
        timestamps[i].resize(1024); // 1024 strips
    }

    Long64_t size = hits->GetEntries(); // size of entries

    // loop through root file and fill timestamp matrix
    for (Long64_t i = 0; i < size; i++)
    {
        hits->GetEntry(i);
        if ((particle.compare(std::string(particle_name)) == 0 || particle.compare(std::string("all")) == 0) 
            && edep > energy_min)
        {
            //histogram with involved particles
            timestamps[det_id][strip_id].push_back(psPerEvent * event_number + time);
            if (std::string(particle_name).compare("proton") == 0 || 
                std::string(particle_name).compare("deuteron") == 0 || 
                std::string(particle_name).compare("triton") == 0 || 
                std::string(particle_name).compare("e-") == 0 || 
                std::string(particle_name).compare("e+") == 0 ||
                std::string(particle_name).compare("gamma") == 0)
                
            {
                hist_particles->Fill(particle_name, 1);
            }
            else
            {
                hist_particles->Fill("other", 1);
            }
        }
    }

    // delta time matrix
    std::vector<std::vector<std::vector<Double_t> > > delta_time;

    // create correct dimension (8 detectors x 1024 strips)
    delta_time.resize(8); // 8 detectors
    for (int i = 0; i < 8; i++)
    {
        delta_time[i].resize(1024); // 1024 strips
    }

    // loop through timestamp matrix and sort entries
    for (int d = 0; d < 8; d++)
    {
        // detector loop
        for (int s = 0; s < 1024; s++)
        {
            // strip loop
            std::sort(timestamps[d][s].begin(), timestamps[d][s].end());

            // loop through times of one specific strip and calculate the
            // delta times. Fill the delta_time matrix with those
            for (int i = 1; i < timestamps[d][s].size(); i++)
            {
                delta_time[d][s].push_back(timestamps[d][s][i] - timestamps[d][s][i - 1]);
            }
        }
    }

    auto stop = std::chrono::system_clock::now();
    cout << "calculating the delta time took: "
         << std::chrono::duration_cast<std::chrono::seconds>(stop - start).count()
         << " s \n";



    //calculating delta time for the asic closest to the beam axis (first or last 128 channels)
    std::vector<std::vector<Double_t>> timestamps_asic1, time_delta_asic1;
    timestamps_asic1.resize(8);
    time_delta_asic1.resize(8);


    for(int d = 0; d < 8; d++){
        if (d < 4){
            for(int s = 0; s < 128; s++){
            timestamps_asic1[d].insert(timestamps_asic1[d].end(), timestamps[d][s].begin(), timestamps[d][s].end());
        }
        std::sort(timestamps_asic1[d].begin(), timestamps_asic1[d].end());
        for (int i = 1; i < timestamps_asic1[d].size(); i++){
            time_delta_asic1[d].push_back(timestamps_asic1[d][i] - timestamps_asic1[d][i-1]);
        }
        } else {
            for(int s = 896; s < 1024; s++){
            timestamps_asic1[d].insert(timestamps_asic1[d].end(), timestamps[d][s].begin(), timestamps[d][s].end());
        }
        std::sort(timestamps_asic1[d].begin(), timestamps_asic1[d].end());
        for (int i = 1; i < timestamps_asic1[d].size(); i++){
            time_delta_asic1[d].push_back(timestamps_asic1[d][i] - timestamps_asic1[d][i-1]);
        }
        }
        
    }

    std::vector<std::vector<Double_t>> stats_asic1;
    stats_asic1.resize(8);
    cout << "---------------------------------------- \n";
    cout << "count rate closest asic (128 channels) \n";
    cout << "detector\tmean\tstdv\tmedian\tlow_quarter\thigh_quarter\thits\t [MHz] \n";
    cout << "---------------------------------------- \n";
    for(int d = 0; d < 8; d++){
        stats_asic1[d] = getStats(time_delta_asic1[d]);
        cout << d
             << "\t" << 1e6 / stats_asic1[d][0]
             << "\t" << 1e6 / stats_asic1[d][1]
             << "\t" << 1e6 / stats_asic1[d][2]
             << "\t" << 1e6 / stats_asic1[d][3]
             << "\t" << 1e6 / stats_asic1[d][4]
             << "\t" << time_delta_asic1[d].size()
             << "\n";
    }



    start = std::chrono::system_clock::now();
    // calculating stats and filling the matrix with them

    std::vector<std::vector<std::vector<Double_t> > > stats;
    // create correct dimension (8 detectors x 1024 strips)
    stats.resize(8); // 8 detectors
    cout << "\n \n\n-----AVERAGE COUNT RATE DETECTORS-----\n";
    cout << "detector\tmean\tstdv\tmedian\tlow_quarter\thigh_quarter\thits\t [MHz] \n";
    // cout << "detector\tmean\tstdv\tmedian\tlow_quarter\thigh_quarter\thits\t [ms] \n";
    cout << "----------\n";
    for (int d = 0; d < 8; d++)
    {
        stats[d].resize(1024); // 1024 strips
        Double_t sum_mean = 0.0;
        Double_t sum_stdv = 0.0;
        Double_t sum_median = 0.0;
        Double_t sum_lq = 0.0;
        Double_t sum_hq = 0.0;

        std::vector<Double_t> detector, detector_delta;
        for (int s = 0; s < 1024; s++)
        {
            stats[d][s] = getStats(delta_time[d][s]);
            detector.insert(detector.end(), timestamps[d][s].begin(), timestamps[d][s].end());
            sum_mean += stats[d][s][0];
            sum_stdv += stats[d][s][1];
            sum_median += stats[d][s][2];
            sum_lq += stats[d][s][3];
            sum_hq += stats[d][s][4];
        }
        std::sort(detector.begin(), detector.end());
        for (Long64_t i = 1; i < detector.size(); i++)
        {
            detector_delta.push_back(detector[i] - detector[i - 1]);
        }
        // std::sort(detector_delta.begin(), detector_delta.end());
        auto s = getStats(detector_delta);
        cout << d
             << "\t" << 1e6/ s[0] 
             << "\t" << 1e6/ s[1] 
             << "\t" << 1e6/ s[2]
             << "\t" << 1e6/ s[3] 
             << "\t" << 1e6/ s[4] 
             //  << "ns    min=" << s[5]/1e3
             //  << "ns    max=" << s[6]/1e3
             << "\t" << detector.size() << "\n";

        // cout << d
        //      << "\t" << sum_mean / (1024e9)
        //      << "\t" << sum_stdv / (1024e9)
        //      << "\t" << sum_median / (1024e9)
        //      << "\t" << sum_lq / (1024e9)
        //      << "\t" << sum_hq / (1024e9)
        //      << "\n";

        // cout << " _______________________ \n";
    }
    cout << "-------------------------------------- \n";

    stop = std::chrono::system_clock::now();
    cout << "calculating stats took: " << std::chrono::duration_cast<std::chrono::seconds>(stop - start).count() << " s \n";

    int total = 0;
    for(int i = 0; i<7; i++){
        total += hist_particles->GetBinContent(i);
    }

    cout << "Contribution from specific particles to the total count rate in % \n \n";
    if(particle.compare("all")==0){
        cout << "total hits \t" << total << "\n";
        cout << "protonen \t" << 100*hist_particles->GetBinContent(hist_particles->GetXaxis()->FindBin("proton"))/total << "\n";
        cout << "electrons \t" << 100*hist_particles->GetBinContent(hist_particles->GetXaxis()->FindBin("e-"))/total << "\n";
        cout << "deuterons \t" << 100*hist_particles->GetBinContent(hist_particles->GetXaxis()->FindBin("deuteron"))/total << "\n";
        cout << "tritons \t" << 100*hist_particles->GetBinContent(hist_particles->GetXaxis()->FindBin("triton"))/total << "\n";
        cout << "gammas \t" << 100*hist_particles->GetBinContent(hist_particles->GetXaxis()->FindBin("gamma"))/total << "\n";
        cout << "positrons \t" << 100*hist_particles->GetBinContent(hist_particles->GetXaxis()->FindBin("e+"))/total << "\n";
        cout << "other \t" << 100*hist_particles->GetBinContent(hist_particles->GetXaxis()->FindBin("other"))/total << "\n";
    } 

    // #####################################################################################################################
    if (draw)
    {
        /* from here on no more calculations are done.
        Only plotting*/

        start = std::chrono::system_clock::now();
        // creating and filling the histograms
        std::vector<TH2D *> histos;
        for (int d = 0; d < 8; d++)
        {
            histos.push_back(new TH2D((std::string("count rate ") +
                                       std::to_string(d))
                                          .c_str(),
                                      (std::string("Detector ") +
                                       std::to_string(d))
                                          .c_str(),
                                      1000, 0, 4, 1024, 0, 1025));

            histos[d]->SetXTitle("count rate [kHz]");
            histos[d]->SetYTitle("strip");
            gStyle->SetOptStat(0);

            for (int s = 0; s < 1024; s += 4)
            { // plot only every 4th strip for resolution reasons
                int delta_time_size = delta_time[d][s].size();
                for (int j = 0; j < delta_time_size; j++)
                {
                    histos[d]->Fill(1e9 / delta_time[d][s][j], s);
                }
            }
        }

        stop = std::chrono::system_clock::now();
        cout << "creating and filling histograms took: " << std::chrono::duration_cast<std::chrono::seconds>(stop - start).count() << " s \n";


        gStyle->SetLabelSize(60, "xyz");
        gStyle->SetTitleSize(70, "xyz");
        gStyle->SetTitleFont(43, "xyz");
        gStyle->SetTitleFontSize(0.08f);
        gStyle->SetLabelFont(43, "xyz");
        gStyle->SetLineWidth(5);
        gStyle->SetFrameLineWidth(5);
        gStyle->SetOptStat(0);


        start = std::chrono::system_clock::now();
        // creating and filling the canvases
        std::vector<TCanvas *> canvases;
        for (int i = 0; i < 4; i++)
        {
            canvases.push_back(new TCanvas((std::string("count_rate_detector_") + std::to_string(i * 2) + std::string("_") + std::to_string(i * 2 + 1) + std::string("_") + particle + std::string("_") + str_energy + std::string("eV_") + draw_opt).c_str(),
                                           (std::string("count_rate_detector_") + std::to_string(i * 2) + std::string("_") + std::to_string(i * 2 + 1) + std::string("_") + particle + std::string("_") + str_energy + std::string("eV_") + draw_opt).c_str()));
            canvases[i]->SetCanvasSize(2880, 1800);
            canvases[i]->SetPadBottomMargin(0.1);
            canvases[i]->SetPadLeftMargin(0.17);
            canvases[i]->SetPadRightMargin(0.02);
            canvases[i]->SetPadTopMargin(0.08);
            canvases[i]->SetTitleOffset(0.12, "t");
            // split each canvas in 2 to display front and rear side
            canvases[i]->Divide(2, 1);
            canvases[i]->cd(1);
            histos[i*2]->SetTitle((std::string("front side det ") + std::to_string(i)).c_str());
            histos[i*2]->GetXaxis()->SetLabelFont(43);
            histos[i*2]->GetXaxis()->SetTitleFont(43);
            histos[i*2]->GetXaxis()->SetTitleSize(70);
            histos[i*2]->GetXaxis()->SetLabelSize(59);
            histos[i*2]->GetYaxis()->SetLabelFont(43);
            histos[i*2]->GetYaxis()->SetTitleFont(43);
            histos[i*2]->GetYaxis()->SetTitleSize(70);
            histos[i*2]->GetYaxis()->SetLabelSize(59);
            histos[i * 2]->Draw(draw_opt.c_str());
            canvases[i]->cd(2);
            histos[i*2+1]->SetTitle((std::string("rear side det ") + std::to_string(i)).c_str());
            histos[i*2+1]->GetXaxis()->SetLabelFont(43);
            histos[i*2+1]->GetXaxis()->SetTitleFont(43);
            histos[i*2+1]->GetXaxis()->SetTitleSize(70);
            histos[i*2+1]->GetXaxis()->SetLabelSize(59);
            histos[i*2+1]->GetYaxis()->SetLabelFont(43);
            histos[i*2+1]->GetYaxis()->SetTitleFont(43);
            histos[i*2+1]->GetYaxis()->SetTitleSize(70);
            histos[i*2+1]->GetYaxis()->SetLabelSize(59);
            histos[i * 2 + 1]->Draw(draw_opt.c_str());
        }

        stop = std::chrono::system_clock::now();
        cout << "creating and filling canvases took: " << std::chrono::duration_cast<std::chrono::seconds>(stop - start).count() << " s \n";

        // draw the average time with errorbars and the total hits
        std::vector<TGraphErrors *> gr_errors;
        std::vector<TGraph *> graphs;
        std::vector<TPad *> tpads;

        for (int d = 0; d < 8; d++)
        {
            std::vector<Double_t> strip, mean, stdv, hits;
            // create vectors with the content to pass to the graphs
            for (int s = 0; s < 1024; s++)
            {
                
                if(timestamps[d][s].size() > 2){    //only calc stats if there are at least two events. Otherwise Div by zero
                    if (std::isinf(1e9/ stats[d][s][0]) || std::isinf(1e9/ stats[d][s][1])){
                    cout << d << "error" << s << "  " << stats[d][s][0] << " " << stats[d][s][1] << endl;
                }
                    strip.push_back(static_cast<double>(s));
                    mean.push_back(1e9 / stats[d][s][0]);
                    stdv.push_back(1e9 / stats[d][s][1]);  
                    hits.push_back(timestamps[d][s].size());
                }
            }
            // to plot different yaxis you need to overlay two different tpads
            tpads.push_back(new TPad((std::string("pad") + std::to_string(d * 2)).c_str(), "", 0, 0, 1, 1));
            tpads.push_back(new TPad((std::string("pad") + std::to_string(d * 2 + 1)).c_str(), "", 0, 0, 1, 1));
            tpads[d * 2 + 1]->SetFillStyle(4000); // makes layer transparent
            tpads[d * 2 + 1]->SetFrameFillStyle(0);

            // gr_errors     contains average delta time with one stdv error
            gr_errors.push_back(new TGraphErrors(strip.size(), &strip[0], &mean[0], 0, &stdv[0]));
            gr_errors[d]->SetName("count rate per strip");
            gr_errors[d]->SetMarkerStyle(4);
            gr_errors[d]->SetMarkerColorAlpha(kAzure - 2, 0.7);
            gr_errors[d]->SetMarkerSize(1.2);
            gr_errors[d]->SetLineColorAlpha(kAzure - 4, 0.1);
            gr_errors[d]->GetXaxis()->SetLabelFont(43);
            gr_errors[d]->GetXaxis()->SetTitleFont(43);
            gr_errors[d]->GetXaxis()->SetTitleSize(70);
            gr_errors[d]->GetXaxis()->SetLabelSize(59);
            gr_errors[d]->SetLineWidth(1);
            gr_errors[d]->GetYaxis()->SetLabelFont(43);
            gr_errors[d]->GetYaxis()->SetTitleFont(43);
            gr_errors[d]->GetYaxis()->SetTitleSize(70);
            //gr_errors[d]->GetYaxis()->SetLineWidth(5);
            gr_errors[d]->GetYaxis()->SetLabelSize(59);
            //gr_errors[d]->GetYaxis()->SetLabelColor(kAzure - 2, 1);
            gr_errors[d]->GetYaxis()->SetTitleColor(kAzure - 2);
            if(d%2 == 0){
                gr_errors[d]->SetTitle((std::string("front side det ") + std::to_string(d/2) + std::string("; strip; count rate [kHz]")).c_str());
            }else {
                gr_errors[d]->SetTitle((std::string("rear side det ") + std::to_string((d-1)/2) + std::string("; strip; count rate [kHz]")).c_str());
            }
            

            // graphs        contains the hits per strip
            graphs.push_back(new TGraph(hits.size(), &strip[0], &hits[0]));
            graphs[d]->SetName("hits per strip");
            graphs[d]->SetMarkerColor(kRed);
            graphs[d]->SetLineColor(kRed);
            graphs[d]->SetLineWidth(1);
            //graphs[d]->GetYaxis()->SetLabelColor(kRed, 1);
            graphs[d]->GetYaxis()->SetTitleOffset(0.6);
            graphs[d]->GetYaxis()->SetTitleColor(kRed);
            //graphs[d]->SetLineColorAlpha(kAzure - 4, 0.1);
            graphs[d]->GetXaxis()->SetLabelFont(43);
            graphs[d]->GetXaxis()->SetTitleFont(43);
            graphs[d]->GetXaxis()->SetTitleSize(70);
            graphs[d]->GetXaxis()->SetLabelSize(59);
            graphs[d]->GetYaxis()->SetLabelFont(43);
            graphs[d]->GetYaxis()->SetTitleFont(43);
            graphs[d]->GetYaxis()->SetTitleSize(70);
            graphs[d]->GetYaxis()->SetLabelSize(59);
            if(d%2 == 0){
                graphs[d]->SetTitle((std::string("front side det ") + std::to_string(d/2) + std::string("; strip   ; hits")).c_str());
            } else {
                graphs[d]->SetTitle((std::string("rear side det ") + std::to_string((d-1)/2) + std::string("; strip   ; hits")).c_str());
            }
        }

        
        gStyle->SetPadBottomMargin(0.15);
        gStyle->SetPadLeftMargin(0.04);
        gStyle->SetPadRightMargin(0.17);
        gStyle->SetPadTopMargin(0.09);
        //gStyle->SetTitleOffset(1.2, "y");
        gStyle->SetTitleOffset(0.12, "t");

        std::vector<TCanvas *> canvases2;
        std::vector<TLegend *> legends;
        for (int i = 0; i < 4; i++)
        {
            canvases2.push_back(new TCanvas((std::string("average count rate per strip detector") + std::to_string(i * 2) + std::string("_") + std::to_string(i * 2 + 1) + std::string("_") + particle + std::string("_") + str_energy + std::string("eV")).c_str(),
                                            (std::string("average_cr_per_strip_") + std::to_string(i * 2) + std::string("_") + std::to_string(i * 2 + 1) + std::string("_") + particle + std::string("_") + str_energy + std::string("eV")).c_str()));
            // split each canvas in 2 to display front and rear side
            canvases2[i]->SetCanvasSize(2880, 1800);
            canvases2[i]->Divide(1, 2);
            canvases2[i]->cd(1);
            tpads[i * 4]->Draw();
            tpads[i * 4]->cd();
            gr_errors[i * 2]->Draw("ALP");
            tpads[i * 4 + 1]->Draw();
            tpads[i * 4 + 1]->cd();
            graphs[i * 2]->Draw("ALY+");

            legends.push_back(new TLegend(0.45, 0.77, 0.7, 0.9));
            legends[i]->AddEntry(gr_errors[i * 2], "count rate per strip with 1 #sigma", "lep"); //with 1 #sigma"
            legends[i]->AddEntry(graphs[i * 2], "hits per strip", "l");
            legends[i]->Draw();

            canvases2[i]->cd(2);
            tpads[i * 4 + 2]->Draw();
            tpads[i * 4 + 2]->cd();
            gr_errors[i * 2 + 1]->Draw("ALP");
            tpads[i * 4 + 3]->Draw();
            tpads[i * 4 + 3]->cd();
            graphs[i * 2 + 1]->Draw("ALY+");
        }

        TCanvas* canvas3 = new TCanvas((std::string("involved particles") + std::string("_") + str_energy + std::string("eV")).c_str(),
                                            (std::string("involved_particles") + std::string("_") + str_energy + std::string("eV")).c_str());
        hist_particles->Draw();
    }

}

// output is structured following {mean, stdv, median, low_quarter,
//                                 high_quarter, minimum, maximum}
std::vector<Double_t> getStats(std::vector<Double_t> &dt)
{
    if (dt.size() > 0)
    {
        std::sort(dt.begin(), dt.end());
        std::vector<Double_t> output;
        // Declare the iterators in a slightly more concise way
        Double_t sum = std::accumulate(dt.begin(), dt.end(), 0.0);

        Double_t mean = sum / dt.size();

        // Can use accumulate here again if you want to be fancy
        Double_t stdev = TMath::Sqrt(
            std::accumulate(
                dt.begin(),
                dt.end(),
                0.0,
                [mean](Double_t sum, Double_t elem)
                { return sum + (elem - mean) * (elem - mean); }) /
            dt.size());

        Double_t min = dt[0];
        Double_t max = dt[dt.size() - 1];

        int s = dt.size();
        Double_t median = dt[s / 2];
        Double_t l_quarter = dt[s / 4];
        Double_t h_quarter = dt[3 * s / 4];

        output.push_back(mean);
        output.push_back(stdev);
        output.push_back(median);
        output.push_back(l_quarter);
        output.push_back(h_quarter);
        output.push_back(min);
        output.push_back(max);
        return output;
    }
    else
    {
        std::vector<Double_t> o = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
        return o;
    }
}