#include <iostream>
#include <fstream>
#include <string>
#include <vector>


using namespace std;

vector <int> input_array;
vector <int> res_array;

vector <int> tag_array; //direct_mapped

vector <int> Tcolumn_0; //four_way_associative
vector <int> Vcolumn_0;
vector <int> Tcolumn_1;
vector <int> Vcolumn_1;
vector <int> Tcolumn_2;
vector <int> Vcolumn_2;
vector <int> Tcolumn_3; //Tag
vector <int> Vcolumn_3; //Victim point

vector <int> tag_fully; //fully_associative
vector <int> victim_pt;

int c_size = 0, b_size = 0, assoc = 0, rep_algo = 0, addr_entries = 0;
int hit_count = 0, miss_count = 0;
int flag_hit, flag_paste;
float miss_rate;


////////////////////////////////////////////////////////////////
void read_file(char *filename){
    ifstream file(filename);
    string line;

    while (getline(file, line)){
        if (line.size() >= 1){
            int line_temp = stoi(line);
            input_array.push_back(line_temp);
        }
    }
}


////////////////////////////////////////////////////////////////
void output_file(string filename){
    filename.erase(filename.begin()+8, filename.end());
    string out_filename = filename + ".out";
    ofstream file(out_filename);

    if (!file.is_open()) {
        cout << "Failed to open file.\n";
    }
    else {
        for (auto &s : res_array) {
            file << s << "\n";
        }
    }
    file << "Miss rate = " << miss_rate;
    file.close();
}


void set_para(int *array){
    c_size = array[0];
    b_size = array[1];
    assoc = array[2];
    rep_algo = array[3];
    addr_entries = (input_array.size() - 4);
}


////////////////////////////////////////////////////////////////////////
void diret_mapped(){
    int b_entries = (c_size/b_size);
    int cur_block = 0, cur_tag = 0;

    //Initialize tag_array and data_array
    for (int j = 0; j < b_entries; j++){
        tag_array.push_back(-1);
    }

    //Caculate
    for (int k = 0; k < addr_entries; k++){
        cur_block = (input_array[k+4] % b_entries); 
        cur_tag = (input_array[k+4] / (b_entries*b_size));

        if (tag_array[cur_block] == cur_tag){
            res_array.push_back(-1); //-1 to res_array (Hit and no victim)
            hit_count++;
        }
        else{ //No hit
            if (tag_array[cur_block] == -1){
                res_array.push_back(-1); //-1 to res_array (Miss but no victim)
                tag_array[cur_block] = cur_tag; //paste new tag
                miss_count++;
            }
            else{
                res_array.push_back(tag_array[cur_block]); //victim to res_array
                tag_array[cur_block] = cur_tag; //paste new tag
                miss_count++;
            }
        }
    }
    miss_rate = float(miss_count) / float(addr_entries);
}


////////////////////////////////////////////////////////////////////////
void four_way_assoc_FIFO(){
    int set_entries = (c_size/(b_size*4));
    int cur_setnum = 0, cur_tag = 0;

    //---------- Initialize tag_array and data_array ----------
    for (int j = 0; j < set_entries; j++){
        Tcolumn_0.push_back(-1);
        Vcolumn_0.push_back(4);
        Tcolumn_1.push_back(-1);
        Vcolumn_1.push_back(3);
        Tcolumn_2.push_back(-1);
        Vcolumn_2.push_back(2);
        Tcolumn_3.push_back(-1);
        Vcolumn_3.push_back(1);
    }

    //------------------------ Caculate ------------------------
    for (int k = 0; k < addr_entries; k++){
        cur_setnum = (input_array[k+4] % set_entries);
        cur_tag = (input_array[k+4] / (set_entries*b_size)); //tag = addr/(sets*b_size)

        if (Tcolumn_0[cur_setnum] == cur_tag){ //Hit
            res_array.push_back(-1);
            hit_count++;
        }
        else if (Tcolumn_1[cur_setnum] == cur_tag){ //Hit
            res_array.push_back(-1);
            hit_count++;
        }
        else if (Tcolumn_2[cur_setnum] == cur_tag){ //Hit
            res_array.push_back(-1);
            hit_count++;
        }
        else if (Tcolumn_3[cur_setnum] == cur_tag){ //Hit
            res_array.push_back(-1);
            hit_count++;
        }
        else{ //No hit
            if (Tcolumn_0[cur_setnum] == -1){
                res_array.push_back(-1); //Miss but no victim
                Tcolumn_0[cur_setnum] = cur_tag; //paste new tag
                miss_count++;
            }
            else if (Tcolumn_1[cur_setnum] == -1){
                res_array.push_back(-1);
                Tcolumn_1[cur_setnum] = cur_tag;
                miss_count++;
            }
            else if (Tcolumn_2[cur_setnum] == -1){
                res_array.push_back(-1);
                Tcolumn_2[cur_setnum] = cur_tag;
                miss_count++;
            }
            else if (Tcolumn_3[cur_setnum] == -1){
                res_array.push_back(-1);
                Tcolumn_3[cur_setnum] = cur_tag;
                miss_count++;
            }
            else{ //No empty blocks and need to select victim
                if (Vcolumn_0[cur_setnum] == 4){
                    res_array.push_back(Tcolumn_0[cur_setnum]);
                    Tcolumn_0[cur_setnum] = cur_tag; //paste new tag
                    miss_count++;
                    Vcolumn_0[cur_setnum] = 0; //reduce victim point
                    Vcolumn_0[cur_setnum] = Vcolumn_0[cur_setnum] + 1; //adjust victim point
                    Vcolumn_1[cur_setnum] = Vcolumn_1[cur_setnum] + 1;
                    Vcolumn_2[cur_setnum] = Vcolumn_2[cur_setnum] + 1;
                    Vcolumn_3[cur_setnum] = Vcolumn_3[cur_setnum] + 1;
                }
                else if (Vcolumn_1[cur_setnum] == 4){
                    res_array.push_back(Tcolumn_1[cur_setnum]);
                    Tcolumn_1[cur_setnum] = cur_tag;
                    miss_count++;
                    Vcolumn_1[cur_setnum] = 0;
                    Vcolumn_0[cur_setnum] = Vcolumn_0[cur_setnum] + 1;
                    Vcolumn_1[cur_setnum] = Vcolumn_1[cur_setnum] + 1;
                    Vcolumn_2[cur_setnum] = Vcolumn_2[cur_setnum] + 1;
                    Vcolumn_3[cur_setnum] = Vcolumn_3[cur_setnum] + 1;
                }
                else if (Vcolumn_2[cur_setnum] == 4){
                    res_array.push_back(Tcolumn_2[cur_setnum]);
                    Tcolumn_2[cur_setnum] = cur_tag;
                    miss_count++;
                    Vcolumn_2[cur_setnum] = 0;
                    Vcolumn_0[cur_setnum] = Vcolumn_0[cur_setnum] + 1;
                    Vcolumn_1[cur_setnum] = Vcolumn_1[cur_setnum] + 1;
                    Vcolumn_2[cur_setnum] = Vcolumn_2[cur_setnum] + 1;
                    Vcolumn_3[cur_setnum] = Vcolumn_3[cur_setnum] + 1;
                }
                else if (Vcolumn_3[cur_setnum] == 4){
                    res_array.push_back(Tcolumn_3[cur_setnum]);
                    Tcolumn_3[cur_setnum] = cur_tag;
                    miss_count++;
                    Vcolumn_3[cur_setnum] = 0;
                    Vcolumn_0[cur_setnum] = Vcolumn_0[cur_setnum] + 1;
                    Vcolumn_1[cur_setnum] = Vcolumn_1[cur_setnum] + 1;
                    Vcolumn_2[cur_setnum] = Vcolumn_2[cur_setnum] + 1;
                    Vcolumn_3[cur_setnum] = Vcolumn_3[cur_setnum] + 1;
                }
            }
        }
    }
    miss_rate = float(miss_count) / float(addr_entries);
}


////////////////////////////////////////////////////////////////////////
void four_way_assoc_LRU(){
    int set_entries = (c_size/(b_size*4));
    int cur_setnum = 0, cur_tag = 0;

    //---------- Initialize tag_array and data_array ----------
    for (int j = 0; j < set_entries; j++){
        Tcolumn_0.push_back(-1);
        Vcolumn_0.push_back(0);
        Tcolumn_1.push_back(-1);
        Vcolumn_1.push_back(0);
        Tcolumn_2.push_back(-1);
        Vcolumn_2.push_back(0);
        Tcolumn_3.push_back(-1);
        Vcolumn_3.push_back(0);
    }

    //------------------------ Caculate ------------------------
    for (int k = 0; k < addr_entries; k++){
        cur_setnum = (input_array[k+4] % set_entries);
        cur_tag = (input_array[k+4] / (set_entries*b_size)); //tag = addr/(sets*b_size)

        if (Tcolumn_0[cur_setnum] == cur_tag){ //Hit
            res_array.push_back(-1); //Hit and no victim
            hit_count++;
            Vcolumn_0[cur_setnum] = 0; //adjust victim point
            Vcolumn_1[cur_setnum] = Vcolumn_1[cur_setnum] + 1;
            Vcolumn_2[cur_setnum] = Vcolumn_2[cur_setnum] + 1;
            Vcolumn_3[cur_setnum] = Vcolumn_3[cur_setnum] + 1;
        }
        else if (Tcolumn_1[cur_setnum] == cur_tag){ //Hit
            res_array.push_back(-1);
            hit_count++;
            Vcolumn_0[cur_setnum] = Vcolumn_0[cur_setnum] + 1;
            Vcolumn_1[cur_setnum] = 0;
            Vcolumn_2[cur_setnum] = Vcolumn_2[cur_setnum] + 1;
            Vcolumn_3[cur_setnum] = Vcolumn_3[cur_setnum] + 1;
        }
        else if (Tcolumn_2[cur_setnum] == cur_tag){ //Hit
            res_array.push_back(-1);
            hit_count++;
            Vcolumn_0[cur_setnum] = Vcolumn_0[cur_setnum] + 1;
            Vcolumn_1[cur_setnum] = Vcolumn_1[cur_setnum] + 1;
            Vcolumn_2[cur_setnum] = 0;
            Vcolumn_3[cur_setnum] = Vcolumn_3[cur_setnum] + 1;
        }
        else if (Tcolumn_3[cur_setnum] == cur_tag){ //Hit
            res_array.push_back(-1);
            hit_count++;
            Vcolumn_0[cur_setnum] = Vcolumn_0[cur_setnum] + 1;
            Vcolumn_1[cur_setnum] = Vcolumn_1[cur_setnum] + 1;
            Vcolumn_2[cur_setnum] = Vcolumn_2[cur_setnum] + 1;
            Vcolumn_3[cur_setnum] = 0;
        }
        else{ //No hit
            if (Tcolumn_0[cur_setnum] == -1){
                res_array.push_back(-1); //Miss but no victim
                Tcolumn_0[cur_setnum] = cur_tag; //paste new tag
                miss_count++;
                Vcolumn_0[cur_setnum] = 0; //adjust victim point
                Vcolumn_1[cur_setnum] = Vcolumn_1[cur_setnum] + 1;
                Vcolumn_2[cur_setnum] = Vcolumn_2[cur_setnum] + 1;
                Vcolumn_3[cur_setnum] = Vcolumn_3[cur_setnum] + 1;
            }
            else if (Tcolumn_1[cur_setnum] == -1){
                res_array.push_back(-1);
                Tcolumn_1[cur_setnum] = cur_tag;
                miss_count++;
                Vcolumn_0[cur_setnum] = Vcolumn_0[cur_setnum] + 1;
                Vcolumn_1[cur_setnum] = 0;
                Vcolumn_2[cur_setnum] = Vcolumn_2[cur_setnum] + 1;
                Vcolumn_3[cur_setnum] = Vcolumn_3[cur_setnum] + 1;
            }
            else if (Tcolumn_2[cur_setnum] == -1){
                res_array.push_back(-1);
                Tcolumn_2[cur_setnum] = cur_tag;
                miss_count++;
                Vcolumn_0[cur_setnum] = Vcolumn_0[cur_setnum] + 1;
                Vcolumn_1[cur_setnum] = Vcolumn_1[cur_setnum] + 1;
                Vcolumn_2[cur_setnum] = 0;
                Vcolumn_3[cur_setnum] = Vcolumn_3[cur_setnum] + 1;
            }
            else if (Tcolumn_3[cur_setnum] == -1){
                res_array.push_back(-1);
                Tcolumn_3[cur_setnum] = cur_tag;
                miss_count++;
                Vcolumn_0[cur_setnum] = Vcolumn_0[cur_setnum] + 1;
                Vcolumn_1[cur_setnum] = Vcolumn_1[cur_setnum] + 1;
                Vcolumn_2[cur_setnum] = Vcolumn_2[cur_setnum] + 1;
                Vcolumn_3[cur_setnum] = 0;
            }
            else{ //No empty blocks and need to select victim
                if ( (Vcolumn_0[cur_setnum] >= Vcolumn_1[cur_setnum]) && (Vcolumn_0[cur_setnum] >= Vcolumn_2[cur_setnum]) && (Vcolumn_0[cur_setnum] >= Vcolumn_3[cur_setnum]) ){
                    res_array.push_back(Tcolumn_0[cur_setnum]);
                    Tcolumn_0[cur_setnum] = cur_tag; //paste new tag
                    miss_count++;
                    Vcolumn_0[cur_setnum] = 0; //adjust victim point
                    Vcolumn_1[cur_setnum] = Vcolumn_1[cur_setnum] + 1;
                    Vcolumn_2[cur_setnum] = Vcolumn_2[cur_setnum] + 1;
                    Vcolumn_3[cur_setnum] = Vcolumn_3[cur_setnum] + 1;
                }
                else if ( (Vcolumn_1[cur_setnum] >= Vcolumn_0[cur_setnum]) && (Vcolumn_1[cur_setnum] >= Vcolumn_2[cur_setnum]) && (Vcolumn_1[cur_setnum] >= Vcolumn_3[cur_setnum]) ){
                    res_array.push_back(Tcolumn_1[cur_setnum]);
                    Tcolumn_1[cur_setnum] = cur_tag;
                    miss_count++;
                    Vcolumn_0[cur_setnum] = Vcolumn_0[cur_setnum] + 1;
                    Vcolumn_1[cur_setnum] = 0;
                    Vcolumn_2[cur_setnum] = Vcolumn_2[cur_setnum] + 1;
                    Vcolumn_3[cur_setnum] = Vcolumn_3[cur_setnum] + 1;
                }
                else if ( (Vcolumn_2[cur_setnum] >= Vcolumn_0[cur_setnum]) && (Vcolumn_2[cur_setnum] >= Vcolumn_1[cur_setnum]) && (Vcolumn_2[cur_setnum] >= Vcolumn_3[cur_setnum]) ){
                    res_array.push_back(Tcolumn_2[cur_setnum]);
                    Tcolumn_2[cur_setnum] = cur_tag;
                    miss_count++;
                    Vcolumn_0[cur_setnum] = Vcolumn_0[cur_setnum] + 1;
                    Vcolumn_1[cur_setnum] = Vcolumn_1[cur_setnum] + 1;
                    Vcolumn_2[cur_setnum] = 0;
                    Vcolumn_3[cur_setnum] = Vcolumn_3[cur_setnum] + 1;
                }
                else if ( (Vcolumn_3[cur_setnum] >= Vcolumn_0[cur_setnum]) && (Vcolumn_3[cur_setnum] >= Vcolumn_1[cur_setnum]) && (Vcolumn_3[cur_setnum] >= Vcolumn_2[cur_setnum]) ){
                    res_array.push_back(Tcolumn_3[cur_setnum]);
                    Tcolumn_3[cur_setnum] = cur_tag;
                    miss_count++;
                    Vcolumn_0[cur_setnum] = Vcolumn_0[cur_setnum] + 1;
                    Vcolumn_1[cur_setnum] = Vcolumn_1[cur_setnum] + 1;
                    Vcolumn_2[cur_setnum] = Vcolumn_2[cur_setnum] + 1;
                    Vcolumn_3[cur_setnum] = 0;
                }
            }
        }
    }
    miss_rate = float(miss_count) / float(addr_entries);
}


////////////////////////////////////////////////////////////////////////
void fully_assoc_FIFO(){
    int blocks = (c_size/b_size);
    int cur_tag = 0;

    //---------- Initialize tag_array and data_array ----------
    for (int j = 0; j < blocks; j++){
        tag_fully.push_back(-1);
        victim_pt.push_back(blocks-j); //e.g. 128, 127, ... ,2 ,1
    }

    //------------------------ Caculate ------------------------
    for (int k = 0; k < addr_entries; k++){
        cur_tag = (input_array[k+4] / b_size); //tag = addr/b_size
        flag_hit = 0;

        for (int m = 0; m < blocks; m++){ //Hit
            if (tag_fully[m] == cur_tag){
                res_array.push_back(-1); //Hit and no victim
                hit_count++;
                flag_hit = 1;
                break;
            }
        }

        if (flag_hit == 0){ //No hit
            flag_paste = 0;
            for (int n = 0; n < blocks; n++){
                if (tag_fully[n] == -1){
                    res_array.push_back(-1); //Miss but no victim
                    tag_fully[n] = cur_tag; //paste new tag
                    miss_count++;
                    flag_paste = 1;
                    break;
                }
            }

            if (flag_paste == 0){ //No empty blocks and need to select victim
                for (int q = 0; q < blocks; q++){
                    if (victim_pt[q] == blocks){
                        res_array.push_back(tag_fully[q]);
                        tag_fully[q] = cur_tag; //paste new tag

                        for (int r = 0; r < blocks; r++){ //adjust victim point
                            victim_pt[r] = victim_pt[r] + 1;
                        }
                        victim_pt[q] = 1; //adjust victim point

                        miss_count++;
                        break;
                    }
                }
            }
        }
    }
    miss_rate = float(miss_count) / float(addr_entries);
}


////////////////////////////////////////////////////////////////////////
void fully_assoc_LRU(){
    int blocks = (c_size/b_size);
    int cur_tag = 0, max_vic_pt = 0, vic_num = 0;

    //---------- Initialize tag_array and data_array ----------
    for (int j = 0; j < blocks; j++){
        tag_fully.push_back(-1);
        victim_pt.push_back(0);
    }

    //------------------------ Caculate ------------------------
    for (int k = 0; k < addr_entries; k++){
        cur_tag = (input_array[k+4] / b_size); //tag = addr/b_size
        flag_hit = 0;

        for (int m = 0; m < blocks; m++){ //Hit
            if (tag_fully[m] == cur_tag){
                res_array.push_back(-1); //Hit and no victim

                for (int x = 0; x < blocks; x++){ //adjust victim point
                    victim_pt[x] = victim_pt[x] + 1;
                }
                victim_pt[m] = 0; //adjust victim point (current)

                hit_count++;
                flag_hit = 1;
                break;
            }
        }

        if (flag_hit == 0){ //No hit
            flag_paste = 0;
            for (int n = 0; n < blocks; n++){
                if (tag_fully[n] == -1){
                    res_array.push_back(-1); //Miss but no victim
                    tag_fully[n] = cur_tag; //paste new tag

                    for (int y = 0; y < blocks; y++){ //adjust victim point
                        victim_pt[y] = victim_pt[y] + 1;
                    }
                    victim_pt[n] = 0; //adjust victim point (current)

                    miss_count++;
                    flag_paste = 1;
                    break;
                }
            }

            if (flag_paste == 0){ //No empty blocks and need to select victim
                for (int r = 0; r < blocks; r++){
                    if (max_vic_pt <= victim_pt[r]){
                        max_vic_pt = victim_pt[r];
                        vic_num = r;
                    }
                }
                res_array.push_back(tag_fully[vic_num]);
                tag_fully[vic_num] = cur_tag; //paste new tag

                for (int z = 0; z < blocks; z++){ //adjust victim point
                    victim_pt[z] = victim_pt[z] + 1;
                }
                victim_pt[vic_num] = 0; //adjust victim point (current)

                miss_count++;
            }
        }
    }
    miss_rate = float(miss_count) / float(addr_entries);
}


////////////////////////////////////////////////////////////////////////
int main(int argc, char **argv){
    read_file(argv[1]);
    set_para(&input_array[0]);


    if (assoc == 0){
        diret_mapped();
    }

    else if(assoc == 1){
        if (rep_algo == 0){
            four_way_assoc_FIFO();
        }
        else if (rep_algo == 1){
            four_way_assoc_LRU();
        }
    }

    else if (assoc == 2){
        if (rep_algo == 0){
            fully_assoc_FIFO();
            //fully_assoc_LRU();
        }
        else if (rep_algo == 1){
            fully_assoc_FIFO();
            //fully_assoc_LRU();
        }
    }

    cout << "Hit: " << hit_count << endl;
    cout << "Miss: " << miss_count << endl;
    cout << "Miss rate = " << miss_rate << endl;

    output_file(argv[1]);

    /*
    for (int i = 0; i < input_array.size(); i++){
        cout << input_array[i] << endl;
    }
    cout << c_size << endl;
    cout << b_size << endl;
    cout << assoc << endl;
    cout << rep_algo << endl;
    cout << addr_entries << endl;
    */
    
    return 0;
}