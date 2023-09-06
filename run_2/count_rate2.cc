#include <TFile.h>
#include <TTree.h>
#include <string.h>
#include <chrono>
#include <TH2D.h>

std::vector<Double_t> getStats(std::vector<Double_t> &dt);

// calculates delta time

void count_rate2(std::string path, std::string particle, std::string draw_opt, bool draw)
{
    auto start = std::chrono::system_clock::now();
    // constants
    Double_t psPerEvent = 1e4; // 10^9 particles/s in ps

    // root file and trees
    TFile *file = new TFile(path.c_str(), "read");
    TTree *hits = (TTree *)file->Get("hits");

    // needed variables from the tree
    char particle_name[128];
    Int_t event_number;
    Int_t det_id, strip_id;
    Double_t time;

    // connect those variables to the tree
    hits->SetBranchAddress("name", &particle_name);
    hits->SetBranchAddress("event", &event_number);
    hits->SetBranchAddress("Det_ID", &det_id);
    hits->SetBranchAddress("Strip_ID", &strip_id);
    hits->SetBranchAddress("Hit_time", &time);

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
        if (particle.compare(std::string(particle_name)) == 0 
            || particle.compare(std::string("all")) == 0)
        {
            timestamps[det_id][strip_id].push_back(psPerEvent * event_number + time);
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

    // loop through timestamp matric and sort entries
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

    start = std::chrono::system_clock::now();
    // calculating stats and filling the matrix with them

    std::vector<std::vector<std::vector<Double_t> > > stats;
    // create correct dimension (8 detectors x 1024 strips)
    stats.resize(8); // 8 detectors
    cout << "-----AVERAGE DELTA TIME DETECTORS----- \n";
    for (int d = 0; d < 8; d++)
    {
        stats[d].resize(1024); // 1024 strips
        Double_t sum_mean = 0.0;
        Double_t sum_stdv = 0.0;
        Double_t sum_median = 0.0;
        for (int s = 0; s < 1024; s++)
        {
            stats[d][s] = getStats(delta_time[d][s]);
            sum_mean += stats[d][s][0];
            sum_stdv += stats[d][s][1];
            sum_median += stats[d][s][2];
        }
        cout << "detector=" << d
             << "    average delta time=" << sum_mean / (1024e9)
             << "ms    average stdv=" << sum_stdv / (1024e9)
             << "ms    average median=" << sum_median / (1024e9) << "ms \n";
    }
    cout << "-------------------------------------- \n";

    stop = std::chrono::system_clock::now();
    cout << "calculating stats took: " << std::chrono::duration_cast<std::chrono::seconds>(stop - start).count() << " s \n";

    if (draw)
    {
        /* from here on no more calculations are done.
        Only plotting*/

        start = std::chrono::system_clock::now();
        // creating and filling the histograms
        std::vector<TH2D *> histos;
        for (int d = 0; d < 8; d++)
        {
            histos.push_back(new TH2D((std::string("delta time ") +
                                       std::to_string(d))
                                          .c_str(),
                                      (std::string("Detector ") +
                                       std::to_string(d))
                                          .c_str(),
                                      1000, 0, 1, 1024, 0, 1025));

            histos[d]->SetXTitle("#Delta t [ms]");
            histos[d]->SetYTitle("strip");
            gStyle->SetOptStat(0);

            for (int s = 0; s < 1024; s += 4)
            { // plot only every 4th strip for resolution reasons
                int delta_time_size = delta_time[d][s].size();
                for (int j = 0; j < delta_time_size; j++)
                {
                    histos[d]->Fill(delta_time[d][s][j] / 1e9, s);
                }
            }
        }

        stop = std::chrono::system_clock::now();
        cout << "creating and filling histograms took: " << std::chrono::duration_cast<std::chrono::seconds>(stop - start).count() << " s \n";

        start = std::chrono::system_clock::now();
        // creating and filling the canvases
        std::vector<TCanvas *> canvases;
        for (int i = 0; i < 4; i++)
        {
            canvases.push_back(new TCanvas((std::string("delta_time_detector_") + std::to_string(i * 2) + std::string("_") + std::to_string(i * 2 + 1) + std::string("_") + draw_opt).c_str(),
                                           (std::string("delta_time_detector_") + std::to_string(i * 2) + std::string("_") + std::to_string(i * 2 + 1) + std::string("_") + draw_opt).c_str()));

            // split each canvas in 2 to display front and rear side
            canvases[i]->Divide(2, 1);
            canvases[i]->cd(1);
            histos[i * 2]->Draw(draw_opt.c_str());
            canvases[i]->cd(2);
            histos[i * 2 + 1]->Draw(draw_opt.c_str());
        }

        stop = std::chrono::system_clock::now();
        cout << "creating and filling canvases took: " << std::chrono::duration_cast<std::chrono::seconds>(stop - start).count() << " s \n";




        // draw the average time with errorbars and the total hits
        std::vector<TGraphErrors*> gr_errors;
        std::vector<TGraph*> graphs;
        std::vector<TPad*> tpads;

        for (int d = 0; d < 8; d++)
        {
            std::vector<Double_t> strip, mean, stdv, hits;

            for (int s = 0; s < 1024; s++)
            {
                strip.push_back(static_cast<double>(s));
                mean.push_back(stats[d][s][0]/1e9);
                stdv.push_back(stats[d][s][1]/1e9);
                hits.push_back(timestamps[d][s].size());
            }
            tpads.push_back(new TPad((std::string("pad") + std::to_string(d*2)).c_str(), "", 0, 0, 1, 1));
            tpads.push_back(new TPad((std::string("pad") + std::to_string(d*2+1)).c_str(), "", 0, 0, 1, 1));
            tpads[d*2+1]->SetFillStyle(4000);   //makes layer transparent
            tpads[d*2+1]->SetFrameFillStyle(0);
            gr_errors.push_back(new TGraphErrors(strip.size(), &strip[0], &mean[0], 0, &stdv[0]));
            gr_errors[d]->SetName("#Delta t per strip");
            gr_errors[d]->SetMarkerColor(kBlue);
            gr_errors[d]->SetMarkerStyle(7);
            gr_errors[d]->SetLineColor(kBlue);
            gr_errors[d]->GetYaxis()->SetLabelColor(kBlue, 1);
            gr_errors[d]->GetYaxis()->SetTitleColor(kBlue);
            gr_errors[d]->SetTitle((std::string("detector ") + std::to_string(d) + std::string("; strip; #Delta t [ms]")).c_str());
            graphs.push_back(new TGraph(hits.size(), &strip[0], &hits[0]));
            graphs[d]->SetMarkerColor(kRed);
            graphs[d]->SetLineColor(kRed);
            graphs[d]->GetYaxis()->SetLabelColor(kRed, 1);
            graphs[d]->GetYaxis()->SetTitleOffset(0.6);
            graphs[d]->GetYaxis()->SetTitleColor(kRed);
            graphs[d]->SetTitle((std::string("detector ") + std::to_string(d) + std::string("; strip; hits")).c_str());
            graphs[d]->SetName("hits per strip");
        }

        std::vector<TCanvas *> canvases2;
        std::vector<TLegend*> legends;
        for (int i = 0; i < 4; i++)
        {
            canvases2.push_back(new TCanvas((std::string("average per strip detector") + std::to_string(i * 2) + std::string("_") + std::to_string(i * 2 + 1) + std::string("_")).c_str(),
                                            (std::string("average_per_strip_") + std::to_string(i * 2) + std::string("_") + std::to_string(i * 2 + 1) + std::string("_")).c_str()));
            // split each canvas in 2 to display front and rear side
            canvases2[i]->Divide(1, 2);
            canvases2[i]->cd(1);
            tpads[i*4]->Draw();
            tpads[i*4]->cd();
            gr_errors[i*2]->Draw("AL");
            tpads[i*4+1]->Draw();
            tpads[i*4 + 1]->cd();
            graphs[i*2]->Draw("ALY+");

            if(i<2){
                legends.push_back(new TLegend(0.75,0.77,0.9,0.9));
                legends[i]->AddEntry(gr_errors[i*2], "#Delta t per strip with 1 #sigma", "le");
                legends[i]->AddEntry(graphs[i*2], "hits per strip", "l");
                legends[i]->Draw();
            }else {
                legends.push_back(new TLegend(0.1,0.77,0.25,0.9));
                legends[i]->AddEntry(gr_errors[i*2], "#Delta t per strip with 1 #sigma", "le");
                legends[i]->AddEntry(graphs[i*2], "hits per strip", "l");
                legends[i]->Draw();
            }

            canvases2[i]->cd(2);
            tpads[i*4+2]->Draw();
            tpads[i*4+2]->cd();
            gr_errors[i*2+1]->Draw("AL");
            tpads[i*4+3]->Draw();
            tpads[i*4 + 3]->cd();
            graphs[i*2 + 1]->Draw("ALY+");
        }
    }
}

// output is structured following {mean, stdv, median, low_quarter,
//                                 high_quarter, minimum, maximum}
std::vector<Double_t> getStats(std::vector<Double_t> &dt)
{
    if (dt.size() > 0)
    {
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