#include<vector>
#include<string>

using namespace std;

class nn_core_data_package_class{  //raw input data used in the input_raw_to_prepared.cpp
    public:
    string label_str;
    vector<vector<float>> data;  
    vector<int> labels; 
    bool analyze_status=false;
    
    void set_no_of_record(int n)
    {
        data.resize(n);
        labels.resize(n);
    }

    void set_no_of_elements_in_each_record(int n)
    {
        for(int a=0;a<data.size();a++)
        {
            data[a].resize(n);
        }
    }
    
    int no_fo_records()
    {
        return data.size();
    }

    int no_of_elements_in_each_record()
    {
        return data[0].size();
    }
};

struct nn_core_filtered_data //its fot the filtered data used in the core class
{
    vector<vector<float>> data;
    string str;
    int label;
};