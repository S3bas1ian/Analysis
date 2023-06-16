#include <TFile.h>
#include <TTree.h>

void ausprobieren()
{

    //File which will be read
    TFile *file = new TFile("mess_1/output_t0.root", "read");
    TTree *hits = (TTree *)file->Get("hits");

    //File, where I write the cluster sizes out
    TFile *outputFile = new TFile("cluster_size.root", "recreate");
    TTree *outputTree = new TTree("cluster_size", "cluster_size");

    int event_size;
    int front_size;
    int back_size;

    outputTree->Branch("event_size", &event_size, "event_size/I");
    outputTree->Branch("front_size", &front_size, "front_size/I");
    outputTree->Branch("back_size", &back_size, "back_size/I");

    char name_hitscoll[128];
    int event_number;
    int det_id;

    hits->SetBranchAddress("Hit_Name", &name_hitscoll);
    hits->SetBranchAddress("event", &event_number);
    hits->SetBranchAddress("Det_ID", &det_id);

    int i=0;
    while (i < hits->GetEntries())
    {
        front_size=0;
        back_size=0;
        hits->GetEntry(i);
        std::string s = "event==" + std::to_string(event_number);
        event_size = hits->GetEntries(s.std::string::c_str());

        for (int j=0; j<event_size; j++){
            hits->GetEntry(i+j);
            if(det_id%2 == 0){
                front_size+=1;
            } else {
                back_size+=1;
            }
        }


        outputTree->Fill();
        i += event_size;
    }

    outputFile->Write();
    outputFile->Close();
}