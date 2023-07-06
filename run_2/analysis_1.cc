void analysis_1(){

    TFile* files[10];
    std::string file_prefix = "mess_1/output_t";
    std::string file_appendix = ".root";

    for (int i=0; i<10; i++){
        files[i] = new TFile(file_prefix + std::to_string(i) + file_appendix);

    }




}