#include "classtype.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <queue>
#include <iomanip> 
#include <regex>
using namespace std;

void readVerilogFile(const string& filename, MODULE* mod, LIB* lib) {
    ifstream infile(filename);
    string line;

    if (!infile) {
        cerr << "Failed to open the file." << endl;
        return;
    }

    bool command_block = false;
    NET* newnet;
    GATE* gate;

    while (getline(infile, line)) {
        
        line = regex_replace(line, regex("/\\*.*?\\*/"), ""); // Remove block comments (/* ... */)

        if (line.find("/*") !=  string::npos) {
            line = regex_replace(line, regex("/\\*.*$"), "");
            command_block = true;
        }

        if (line.find("*/") != string::npos) {
            line = regex_replace(line, regex("^.*\\*/"), "");
            command_block = false;
        }

        line = regex_replace(line, regex("\\s+"), " "); // Remove extra spaces and tabs

        // Remove inline comments (// ...)
        line = regex_replace(line, regex("//.*"), "");

        line = regex_replace(line, regex("^\\s+|\\s+$"), ""); // Remove leading and trailing whitespaces

        if (line.empty()) {
            continue; // Skip empty lines
        }

        if (!command_block) {
            if (line.find("module ") != string::npos) {
            // Extract module name, input, and output signals
                regex module_regex("module\\s+(\\w+)\\s*\\(([^)]+)\\s*\\)\\s*;");
                smatch module_match;
                if (regex_match(line, module_match, module_regex)) {
                    mod->Name = module_match[1]; // module name
                    string signals = module_match[2]; // input and output signals
                    regex signal_regex("\\s*(\\w+),?");
                    smatch signal_match;
                    while (regex_search(signals, signal_match, signal_regex)) {
                        // mod->AllSignals.push_back(signal_match[1]);             // comment
                        signals = signal_match.suffix();
                    }
                }
            }
            else if (line.find("output") != string::npos) {
                regex output_regex("output\\s+([^)]+)\\s*;");
                smatch output_match;
                if (regex_match(line, output_match, output_regex)) {
                    string signals = output_match[1]; // input and output signals
                    regex signal_regex("\\s*(\\w+),?");
                    smatch signal_match;
                    while (regex_search(signals, signal_match, signal_regex)) {
                        newnet = new NET(signal_match[1], 1);
                        mod->OutputSignals.push_back(newnet);             // comment
                        mod->Netlist.insert(pair<string, NET*>(signal_match[1], newnet));
                        signals = signal_match.suffix();
                    }
                }
            }
            else if (line.find("input") != string::npos) {
                regex input_regex("input\\s+([^)]+)\\s*;");
                smatch input_match;
                if (regex_match(line, input_match, input_regex)) {
                    string signals = input_match[1]; // input and output signals
                    regex signal_regex("\\s*(\\w+),?");
                    smatch signal_match;
                    while (regex_search(signals, signal_match, signal_regex)) {
                        newnet = new NET(signal_match[1], 0);
                        mod->InputSignals.push_back(newnet);             // comment
                        mod->Netlist.insert(pair<string, NET*>(signal_match[1], newnet));
                        signals = signal_match.suffix();
                    }
                }
            }
            else if (line.find("wire") != string::npos) {
                regex wire_regex("wire\\s+([^]+);\\s*");
                smatch wire_match;
                if (regex_match(line, wire_match, wire_regex)) {
                    string signals = wire_match[1]; // input and output signals
                    regex signal_regex("\\s*(\\w+),?");
                    smatch signal_match;
                    while (regex_search(signals, signal_match, signal_regex)) {
                        newnet = new NET(signal_match[1],2);
                        mod->WireSignals.push_back(newnet);             // comment
                        mod->Netlist.insert(pair<string, NET*>(signal_match[1], newnet));
                        signals = signal_match.suffix();
                    }
                }
            }
            else {
                // Extract gate information
                regex gate_regex("((\\w+)\\s*(\\w+)\\s*\\(([^;]+))\\);");
                smatch gate_match;
                if (regex_match(line, gate_match, gate_regex)) {
                    gate = new GATE;
                    gate->Type = lib->CellTypeList[gate_match[2]];
                    gate->Name = gate_match[3];
                    string gate_nets = gate_match[4];

                    regex net_regex("\\s*\\.(\\w+)\\s*\\(\\s*(\\w+)\\s*\\),?");
                    smatch net_match;
                    while (regex_search(gate_nets, net_match, net_regex)) {
                        string tmp = net_match[1];
                        if (tmp[0] == 'Z' && tmp[1] == 'N') {
                            if (!mod->Netlist[net_match[2]]->GateOutputList.empty()) {
                                for (GATE* outgate : mod->Netlist[net_match[2]]->GateOutputList) {
                                    outgate->FanIn.insert(pair<string, GATE*>(outgate->Inputs[net_match[2]], gate));
                                    gate->FanOut.push_back(pair<string, GATE*>(outgate->Inputs[net_match[2]], outgate));
                                }
                            }
                            mod->Netlist[net_match[2]]->GateInput = gate;
                            gate->Output = mod->Netlist[net_match[2]];
                        }
                        else {
                            if (mod->Netlist[net_match[2]]->GateInput != nullptr) {
                                mod->Netlist[net_match[2]]->GateInput->FanOut.push_back(pair<string, GATE*>(net_match[1], gate));
                                gate->FanIn.insert(pair<string, GATE*>(net_match[1], mod->Netlist[net_match[2]]->GateInput));
                            }
                            gate->Inputs.insert(pair<string, string>(net_match[2], net_match[1])); // NET* = mod->Netlist.find(net_match[2])->second
                            gate->InputNets.insert(pair<string, NET*>(net_match[1], mod->Netlist[net_match[2]]));
                            mod->Netlist[net_match[2]]->GateOutputList.push_back(gate);
                        }
                        gate_nets = net_match.suffix();
                    }
                    mod->GateList.push_back(gate);
                }
            }
        }
    }

    // // Print module information
    // cout << "Module: " << mod->Name << endl;
    // cout << "All signals: ";
    // for (const string& input : mod->AllSignals) {
    //     cout << input << " ";
    // }
    // cout << endl;
    // cout << "Input signals: ";
    // for (const string& input : mod->InputSignals) {
    //     cout << input << " ";
    // }
    // cout << endl;
    // cout << "Output signals: ";
    // for (const string& output : mod->OutputSignals) {
    //     cout << output << " ";
    // }
    // cout << endl;
    // cout << "Wire signals: ";
    // for (const string& wire : mod->WireSignals) {
    //     cout << wire << " ";
    // }
    // cout << endl;
    
    // // Print gate information
    // for (GATE* gate : mod->GateList) {
    //     cout << "Gate Name: " << gate->Name << endl;
    //     cout << "Output: " << gate->Output->Name << endl;
    //     for (auto& nextgate : gate->FanIn) {
    //         cout << nextgate.second->Name << " ";
    //     }
    //     cout << endl;
    //     for (auto& nextgate : gate->FanOut) {
    //         cout << nextgate.second->Name << " ";
    //     }
        
    //     cout << endl;
    // }

    return;
}

void readLibFile(ifstream& file, LIB* lib) {
    string line;
    bool command_block;
    while(getline(file, line)) {
        line = regex_replace(line, regex("/\\*.*?\\*/"), ""); // Remove block comments (/* ... */)
        
        if (line.find("/*") !=  string::npos) {
            line = regex_replace(line, regex("/\\*.*$"), "");
            command_block = true;
        }

        if (line.find("*/") != string::npos) {
            line = regex_replace(line, regex("^.*\\*/"), "");
            command_block = false;
        }

        line = regex_replace(line, regex("\\s+"), " "); // Remove extra spaces and tabs
        line = regex_replace(line, regex("//.*"), ""); // Remove inline comments (// ...)
        line = regex_replace(line, regex("^\\s+|\\s+$"), ""); // Remove leading and trailing whitespaces

        if (line.empty()) {
            continue; // Skip empty lines
        }

        if (!command_block) {
            smatch tmp_match;
            string tmp_string, str;
            size_t start, end;
            bool readcell = false, readpin = false;

            regex time_regex("time_unit\\s*:\\s*\"(\\w+)\";");
            regex voltage_regex("voltage_unit\\s*:\\s*\"(\\w+)\";");
            regex current_regex("current_unit\\s*:\\s*\"(\\w+)\";");
            regex capacitive_load_regex("capacitive_load_unit\\s*:\\s*\\((\\w+)\\);");
            regex var1_regex("variable_1\\s*:\\s*(\\w+)\\s*;");
            regex var2_regex("variable_2\\s*:\\s*(\\w+)\\s*;");
            regex index1_regex("index_1\\s\\(\".+\"\\);");
            regex index2_regex("index_2\\s\\(\".+\"\\);");
            regex cell_regex("cell\\s*\\((\\w+)\\)\\s*\\{");
            regex pin_regex("\\s*pin\\s*\\((\\w+)\\)\\s*\\{\\s*");

            if (regex_match(line, tmp_match, time_regex)) lib->TimeUnit = tmp_match[1];
            else if (regex_match(line, tmp_match, voltage_regex)) lib->VoltageUnit = tmp_match[1];
            else if (regex_match(line, tmp_match, current_regex)) lib->CurrentUnit = tmp_match[1];
            else if (regex_match(line, tmp_match, capacitive_load_regex)) lib->CapacitiveLoadUnit = tmp_match[1];
            else if (regex_match(line, tmp_match, var1_regex)) lib->Variable1 = tmp_match[1];
            else if (regex_match(line, tmp_match, var2_regex)) lib->Variable2 = tmp_match[1];
            else if (regex_match(line, tmp_match, index1_regex)) {
                tmp_string = line.substr(line.find("(")+2, line.find(")")-line.find("(")-3);
                stringstream ss(tmp_string);
                while (getline(ss, str, ',')) lib->Index1.push_back(stod(str));
            }
            else if (regex_match(line, tmp_match, index2_regex)) {
                tmp_string = line.substr(line.find("(")+2, line.find(")")-line.find("(")-3);
                stringstream ss(tmp_string);
                while (getline(ss, str, ',')) lib->Index2.push_back(stod(str));
            }
            else if (regex_match(line, tmp_match, cell_regex)) {
                CELLTYPE* celltype = lib->NewCellType(tmp_match[1]);
                readcell = false;
                while (!readcell) {
                    getline(file, line);
                    if (regex_match(line, tmp_match, pin_regex)) {
                        if (tmp_match[1].str()[0] == 'Z') {
                            PINTYPE* pintype = celltype->NewPinType(tmp_match[1], false);
                            readpin = false;
                            while (!readpin) {
                                getline(file, line);
                                if (line.find("direction") != string::npos) continue;
                                else if (line.find("capacitance") != string::npos) {
                                    str = line.substr(line.find(":")+2, line.find(";")-line.find(":")-2);
                                    pintype->Capacitance = stod(str);
                                }
                                else {
                                    while (getline(file, line)) {
                                        if (line.find("rise_power") != string::npos) {
                                            while (getline(file, line)) {
                                                if (line.find("}") != string::npos) break;
                                                tmp_string = line.substr(line.find("\"")+1, line.find("\\")-line.find("\"")-3);
                                                stringstream ss(tmp_string);
                                                vector<double> tmpvec;
                                                while (getline(ss, str, ',')) tmpvec.push_back(stod(str));
                                                celltype->PowerRiseTable.push_back(tmpvec);

                                            }
                                        }
                                        else if (line.find("fall_power") != string::npos) {
                                            while (getline(file, line)) {
                                                if (line.find("}") != string::npos) break;
                                                tmp_string = line.substr(line.find("\"")+1, line.find("\\")-line.find("\"")-3);
                                                stringstream ss(tmp_string);
                                                vector<double> tmpvec;
                                                while (getline(ss, str, ',')) tmpvec.push_back(stod(str));
                                                celltype->PowerFallTable.push_back(tmpvec);
                                            }
                                        }
                                        else if (line.find("cell_rise") != string::npos) {
                                            while (getline(file, line)) {
                                                if (line.find("}") != string::npos) break;
                                                tmp_string = line.substr(line.find("\"")+1, line.find("\\")-line.find("\"")-3);
                                                stringstream ss(tmp_string);
                                                vector<double> tmpvec;
                                                while (getline(ss, str, ',')) tmpvec.push_back(stod(str));
                                                celltype->CellRiseTable.push_back(tmpvec);
                                            }
                                        }
                                        else if (line.find("cell_fall") != string::npos) {
                                            while (getline(file, line)) {
                                                if (line.find("}") != string::npos) break;
                                                tmp_string = line.substr(line.find("\"")+1, line.find("\\")-line.find("\"")-3);
                                                stringstream ss(tmp_string);
                                                vector<double> tmpvec;
                                                while (getline(ss, str, ',')) tmpvec.push_back(stod(str));
                                                celltype->CellFallTable.push_back(tmpvec);
                                            }
                                        }
                                        else if (line.find("rise_transition") != string::npos) {
                                            while (getline(file, line)) {
                                                if (line.find("}") != string::npos) break;
                                                tmp_string = line.substr(line.find("\"")+1, line.find("\\")-line.find("\"")-3);
                                                stringstream ss(tmp_string);
                                                vector<double> tmpvec;
                                                while (getline(ss, str, ',')) tmpvec.push_back(stod(str));
                                                celltype->TransitionRiseTable.push_back(tmpvec);
                                            }
                                        }
                                        else if (line.find("fall_transition") != string::npos) {
                                            while (getline(file, line)) {
                                                if (line.find("}") != string::npos) break;
                                                tmp_string = line.substr(line.find("\"")+1, line.find("\\")-line.find("\"")-3);
                                                stringstream ss(tmp_string);
                                                vector<double> tmpvec;
                                                while (getline(ss, str, ',')) tmpvec.push_back(stod(str));
                                                celltype->TransitionFallTable.push_back(tmpvec);
                                            }
                                            readpin = true;
                                            readcell = true;
                                            break;
                                        }
                                    }
                                }
                            }
                        }
                        else {
                            PINTYPE* pintype = celltype->NewPinType(tmp_match[1], true);
                            readpin = false;
                            while (!readpin) {
                                getline(file, line);
                                if (line.find("direction") != string::npos) continue;
                                else if (line.find("capacitance") != string::npos) {
                                    str = line.substr(line.find(":")+2, line.find(";")-line.find(":")-2);
                                    pintype->Capacitance = stod(str);
                                    readpin = true;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    return;
}

void readPatFile(ifstream& file, MODULE* mod, vector<vector<pair<string, bool>>>& pat) {
    string line;
    vector<string> inputOrder;
    
    getline(file, line);
    size_t commaPos = line.find(",", 0);
    while (commaPos != string::npos) {
        inputOrder.push_back(line.substr(line.rfind(" ", commaPos) + 1, commaPos - line.rfind(" ", commaPos) -1 ));
        commaPos = line.find(",", commaPos+1); 
    }
    inputOrder.push_back(line.substr(line.rfind(",", line.size()) + 2, line.size() - 1 - line.rfind(",", line.size())));
    // cout << "===========" << endl;
    // for (string order : inputOrder) {
    //     cout << order << " ";
    // }
    // cout << endl << "============" << endl;
    int i = 0;
    vector<pair<string, bool>> input;
    file >> line;
    while (line.compare(".end") != 0) {
        input.push_back(pair<string, bool>(inputOrder[i], ((line.compare("0") == 0) ? false : true)));
        // cout << i << " " << ((line.compare("0") == 0) ? false : true) << endl;
        if (i == inputOrder.size()-1) {
            pat.push_back(input);
            i = 0;
            input.clear();
        }
        else i++;
        file >> line;
    }
}

bool compareByLoad(const GATE* a, const GATE* b) {
    if (a->OutCap != b->OutCap) {
        return a->OutCap > b->OutCap;
    }
    return stoi(a->Name.substr(1)) < stoi(b->Name.substr(1));
}

bool compareByDelay(const GATE* a, const GATE* b) {
    if (a->Delay != b->Delay) {
        return a->Delay > b->Delay;
    }
    return stoi(a->Name.substr(1)) < stoi(b->Name.substr(1));
}

int main(int argc, char* argv[]) {
    
    if (argc != 5) {
        cerr << "Usage: " << argv[0] << " netlist_file -l test_lib.lib" << endl;
        return 1;
    }
    LIB lib;
    // Check if the -l option is provided
    if (string(argv[2]) == "-l") {
        string libFilename = argv[3];
        // Read the .lib file here and process it as needed
        ifstream libFile(libFilename);
        if (!libFile) {
            cerr << "Failed to open the .lib file." << endl;
            return 1;
        }
        cout << "Read .lib File";
        readLibFile(libFile, &lib);
        cout << " done." << endl;
    }
    // Read the .v file 
    string verilogFilename = argv[1];
    MODULE mod;
    cout << "Read .v File";
    readVerilogFile(verilogFilename, &mod, &lib);
    cout << " done." << endl;

    vector<vector<pair<string, bool>>> Pattern;
    cout << "Read .pat File";
    ifstream patFile(argv[4]);
    readPatFile(patFile, &mod, Pattern);
    cout << " done." << endl;

    verilogFilename = verilogFilename.substr(0, verilogFilename.length() - 2);
    ofstream loadFile("312511052_" + verilogFilename + "_load.txt");
    ofstream delayFile("312511052_" + verilogFilename + "_gate_info.txt");
    ofstream powerFile("312511052_" + verilogFilename + "_gate_power.txt");
    ofstream coverageFile("312511052_" + verilogFilename + "_coverage.txt");

    // calculation begin
    queue<GATE*> TopologicalSort;
    vector<GATE*> SortedGateList;
    GATE* gate;
    double totalPower, toggleCoverage, toggleNum;
    // cout << Pattern.size() << endl;
    for (int i=0; i<Pattern.size(); i++) {
        cout << "============== " << i << " ==============" << endl;
        totalPower = 0;
        toggleNum = 0;
        for (auto& pair : Pattern[i]) {
            mod.Netlist[pair.first]->Value = pair.second;
            // cout << mod.Netlist[pair.first]->Name << " " << pair.second << endl;
        }

        if (i == 0) {
            for (NET* InputNet : mod.InputSignals) {
                for (GATE* gate : InputNet->GateOutputList) {
                    TopologicalSort.push(gate);
                    gate->Ready = (gate->FanIn.empty()) ? true : false;
                }
            }
        
            while (!TopologicalSort.empty()) {
                gate = TopologicalSort.front();
                // cout << gate->Name << " " << gate->Ready << gate->Done << endl;
                if (gate->Ready && !gate->Done) {
                    gate->CalOutputValue();
                    gate->getOutputCapAndSwitchingPower();
                    gate->getDelayAndPower(&lib, false); // should be done first time  
                    for (auto& nextgate : gate->FanOut) {
                        TopologicalSort.push(nextgate.second);
                    }
                    SortedGateList.push_back(gate);
                    totalPower += gate->InternalPower;
                    if (gate->toggle) totalPower += gate->SwitchingPower;
                }
                TopologicalSort.pop(); 
            }
        }
        else {
            for (int j=0; j< SortedGateList.size(); j++) {
                gate = SortedGateList[j];
                gate->Clean();
                gate->CalOutputValue();
                gate->getDelayAndPower(&lib, false);
                totalPower += gate->InternalPower;
                if (gate->toggle) {
                    totalPower += gate->SwitchingPower;
                }
            }
        }

        for (GATE* gate : mod.GateList) {
            delayFile << gate->Name << " " << gate->rise << " " << fixed <<  setprecision(6) << gate->Delay << " " << gate->TranDelay << endl;
            powerFile << gate->Name << " " << fixed << setprecision(6) << gate->InternalPower << " " << gate->SwitchingPower << endl;
            toggleNum = gate->Toggle0to1 + gate->Toggle1to0 + toggleNum;
        }
        toggleCoverage = toggleNum / (40 * mod.GateList.size()) * 100;
        coverageFile << i+1 << " " << fixed << setprecision(6) << totalPower << " " << fixed << setprecision(2) << toggleCoverage << "%" << endl;

        delayFile << endl;
        powerFile << endl;
        coverageFile << endl;
    }
    cout << "sorted" << endl;
    
    if (loadFile.is_open()) {
        // sort(mod.GateList.begin(), mod.GateList.end(), compareByLoad);
        for (int i=0; i<mod.GateList.size(); i++) {
            loadFile << mod.GateList[i]->Name << " " << fixed <<  setprecision(6) << mod.GateList[i]->OutCap << endl;
        }
        loadFile.close();
        // cout << "success" << endl;
    }
    // else cout << "error" << endl;

    // sort(mod.GateList.begin(), mod.GateList.end(), compareByDelay);
    

    return 0;
}
