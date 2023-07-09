#include <TFile.h>
#include <TGraph.h>
#include <TTree.h>
#include <string.h>
#include <vector>

void count_rate()
{

	int det[] = {0, 1, 7, 8};
	int strip[] = {0, 1, 2, 200, 555, 700, 1021, 1022, 1023};
	double e_min = 100000; // unit eV

	double psPerEvent = 1e4; //unit pS
	vector<double> vec_time_point;

	TFile *file = new TFile("data/final_output/output3.root", "read");
	TTree *hits = (TTree *)file->Get("hits");

	//variables that are connected to the root file
	char particle_name[128];
	int event_number;
	int det_id;
	int strip_id;
	double edep;
	double time;

	//connect variables with the branch from the root file
	hits->SetBranchAddress("name", &particle_name);
	hits->SetBranchAddress("event", &event_number);
	hits->SetBranchAddress("Det_ID", &det_id);
	hits->SetBranchAddress("Strip_ID", &strip_id);
	hits->SetBranchAddress("edep", &edep);
	hits->SetBranchAddress("time", &time);

	// preload stuff to speed things up
	hits->LoadBaskets();

	//iterate through the wanted detectors and strips
	for (int d : det)
	{
		for (int s : strip)
		{
			double* o;

			//calculate for each det/strip combnination the mean and stdv
			o = get_Mean_and_Stdv(d, s);
			cout << "------------ detector: " << d << "and strips: " << s << "------------"  << endl;
			cout << "average [ps]: " << *o << endl;
			cout << "std deviation [ps]: " << *(o+1) << endl;
		}
	}

	// auto c1 = new TCanvas("c", "c", 800, 800);
	// // g->SetTitle("Time between hits in specific strip");
	// h->Draw();
	// c1->SaveAs("time_elapsed.png");
}

double* get_Mean_and_Stdv(int det_id, int strip_id)	//should return mean and stdv
{
	

	int size = this.hits->GetEntries();

	//iterate through the whole root file
	for (int i = 0; i < size; i++)
	{
		this.hits->GetEntry(i);

		//if the current entry is on the specific strip
		if (this.det_id == det_id && this.strip_id == strip_id)
		{
			//then add the time point to the vector
			double t = this.psPerEvent * this.event_number + this.time;
			this.vec_time_point.push_back(t);
		}
	}

	//sort the time point vector
	std::sort(this.vec_time_point.begin(), this.vec_time_point.end());

	vector<double> delta_time;
	//auto h = new TH1D("hist", "delta time", 100, 0, 2);

	//calculate the time between two events
	for (int i = 0; i < vec_time_point.size() - 1; i++)
	{
		double t = this.vec_time_point[i + 1] - this.vec_time_point[i];
		delta_time.push_back(t);
		//h->Fill(t);
	}

	//calculate mean and stdv and return it
	double sum = std::accumulate(std::begin(delta_time), std::end(delta_time), 0.0);
	double m = sum / delta_time.size();

	double accum = 0.0;
	std::for_each(std::begin(delta_time), std::end(delta_time), [&](const double d)
				  { accum += (d - m) * (d - m); });

	double stdev = sqrt(accum / (delta_time.size() - 1));
	double out[] = {m, stdev};
	return *out;
}
