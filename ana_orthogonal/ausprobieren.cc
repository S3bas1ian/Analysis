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

    outputTree->Branch("event_size", &event_size, "event_size/I");

    char name_hitscoll[128];
    int event_number;

    hits->SetBranchAddress("Hit_Name", &name_hitscoll);
    hits->SetBranchAddress("event", &event_number);

    int i=0;
    while (i < hits->GetEntries())
    {
        hits->GetEntry(i);
        std::string s = "event==" + std::to_string(event_number);
        event_size = hits->GetEntries(s.std::string::c_str());
        outputTree->Fill();
        i += event_size;
    }

    outputFile->Write();
    outputFile->Close();
}