void ausprobieren(){

    TFile* file = new TFile("mess_1/output_t0.root", "read");
    TTree* hits = (TTree*) file->Get("hits");

    char name_hitscoll [128];

    hits->SetBranchAddress("Hit_Name", &name_hitscoll);

    hits->GetEntry(0);
    cout << name_hitscoll << endl;
}