#include <TFile.h>
#include <TTree.h>
#include <string.h>
#include <chrono>
#include <TH2D.h>

// calculates delta time

void count_rate2(std::string path, std::string particle, std::string draw_opt)
{
    auto start = std::chrono::system_clock::now();
    //constants
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
    

    //timestamps[detector][strip][entry] as matrix
    //Needed when looping only once through all entries from root file
    std::vector<std::vector<std::vector<Double_t>>> timestamps;

    //create correct dimension (8 detectors x 1024 strips)
    timestamps.resize(8);    //8 detectors
    for(int i=0; i<8; i++){
        timestamps[i].resize(1024); //1024 strips
    }

    Long64_t size = hits->GetEntries(); //size of entries

    //loop through root file and fill timestamp matrix
    for(Long64_t i = 0; i<size; i++){
        hits->GetEntry(i);
        if(particle.compare(std::string(particle_name)) == 0 
            || particle.compare(std::string("all")) == 0){
                timestamps[det_id][strip_id].push_back(psPerEvent * event_number + time);
            }
    }

    //delta time matrix
    std::vector<std::vector<std::vector<Double_t>>> delta_time;

    //create correct dimension (8 detectors x 1024 strips)
    delta_time.resize(8);    //8 detectors
    for(int i=0; i<8; i++){
        delta_time[i].resize(1024); //1024 strips
    }

    //loop through timestamp matric and sort entries
    for(int d = 0; d<8; d++){
        //detector loop
        for(int s = 0; s<1024; s++){
            //strip loop
            std::sort(timestamps[d][s].begin(), timestamps[d][s].end());

            //loop through times of one specific strip and calculate the 
            //delta times. Fill the delta_time matrix with those
            for(int i = 1; i<timestamps[d][s].size() ; i++){
                delta_time[d][s].push_back(timestamps[d][s][i] - timestamps[d][s][i-1]);
            }
        }
    }

    auto stop = std::chrono::system_clock::now();
    cout << "calculating the delta time took took: " << 
            std::chrono::duration_cast<std::chrono::seconds>(stop - start).count() << " s \n";



    start = std::chrono::system_clock::now();
    //creating and filling the histograms
    std::vector<std::shared_ptr<TH2D>> histos;
    for(int d = 0; d<8; d++){
        histos.push_back(std::make_shared<TH2D>((std::string("delta time ") + 
                std::to_string(d)).c_str(), 
                (std::string("#Delta t for detector ") + 
                std::to_string(d)).c_str(), 1000, 0, 2, 1024, 0, 1025));

        histos[d]->SetXTitle("#Delta t [ms]");
        histos[d]->SetYTitle("strip");

        for(int s = 0; s<1024; s+=2){   //plot only every second strip for resolution reasons
            int delta_time_size = delta_time[d][s].size();
            for(int j = 0; j<delta_time_size; j++){
                histos[d]->Fill(delta_time[d][s][j]/1e9, s);
            }
        }
    }

    stop = std::chrono::system_clock::now();
    cout << "creating and filling histograms took: " << 
            std::chrono::duration_cast<std::chrono::seconds>(stop - start).count() << " s \n";


    start = std::chrono::system_clock::now();
    //creating and filling the canvases
    std::vector<std::shared_ptr<TCanvas>> canvases;
    for(int i = 0; i<4; i++){
        canvases.push_back(std::make_shared<TCanvas>((std::string("delta_time_detector_") 
            + std::to_string(i*2) + std::string("_") + std::to_string(i*2+1) 
            + std::string("_") + draw_opt).c_str(), 
            (std::string("delta_time_detector_") + std::to_string(i*2) 
            + std::string("_") + std::to_string(i*2+1) + std::string("_") 
            + draw_opt).c_str()));


        canvases[i]->Divide(2, 1);
        //canvases[i]->SetCanvasSize(1500, 1000);
        canvases[i]->cd(1);
        histos[i*2]->Draw(draw_opt.c_str());
        canvases[i]->cd(2);
        histos[i*2 + 1]->Draw(draw_opt.c_str());
    }

    stop = std::chrono::system_clock::now();
    cout << "creating and filling canvases took: " << 
            std::chrono::duration_cast<std::chrono::seconds>(stop - start).count() << " s \n";
}