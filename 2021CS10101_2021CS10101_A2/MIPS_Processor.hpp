#ifndef __MIPS_PROCESSOR_HPP__
#define __MIPS_PROCESSOR_HPP__

#include <unordered_map>
#include <string>
#include <functional>
#include <vector>
#include <fstream>
#include <exception>
#include <iostream>
#include <boost/tokenizer.hpp>
using namespace std;

struct MIPS_Architecture
{
	int registers[32] = {0};
	int PCcurr = 0,PCnext;
	int register_store[5][32]= {0};
	std::vector<std::vector<std::string>> output;
	std::vector<std::vector<int>> outputh;
	std::vector<pair<std::string,int>> under_use;
	std::unordered_map<std::string, std::function<int(MIPS_Architecture &, std::string, std::string, std::string)>> instructions;
	std::unordered_map<std::string, int> registerMap, address;
	static const int MAX = (1 << 20);
	int data[MAX >> 2] = {0};
	int dc = 0;
	vector <pair<int,int>> optdata = {};
	std::vector<std::vector<std::string>> commands;
	std::vector<int> commandCount;
	enum exit_code
	{
		SUCCESS = 0,
		INVALID_REGISTER,
		INVALID_LABEL,
		INVALID_ADDRESS,
		SYNTAX_ERROR,
		MEMORY_ERROR
	};

	// constructor to initialise the instruction set
	MIPS_Architecture(std::ifstream &file)
	{
		instructions = {{"add", &MIPS_Architecture::add}, {"sub", &MIPS_Architecture::sub}, {"mul", &MIPS_Architecture::mul}, {"beq", &MIPS_Architecture::beq}, {"bne", &MIPS_Architecture::bne}, {"slt", &MIPS_Architecture::slt}, {"j", &MIPS_Architecture::j}, {"lw", &MIPS_Architecture::lw}, {"sw", &MIPS_Architecture::sw}, {"addi", &MIPS_Architecture::addi}};
		output = {{"IF","0","",""},{"ID","0","",""},{"EX","0","",""},{"MEM","0","",""},{"WB","0","",""}};
		outputh = {{0,0},{0,0},{0,0},{0,0},{0,0}};
		under_use = {};
		for (int i = 0; i < 32; ++i)
			registerMap["$" + std::to_string(i)] = i;
		registerMap["$zero"] = 0;
		registerMap["$at"] = 1;
		registerMap["$v0"] = 2;
		registerMap["$v1"] = 3;
		for (int i = 0; i < 4; ++i)
			registerMap["$a" + std::to_string(i)] = i + 4;
		for (int i = 0; i < 8; ++i)
			registerMap["$t" + std::to_string(i)] = i + 8, registerMap["$s" + std::to_string(i)] = i + 16;
		registerMap["$t8"] = 24;
		registerMap["$t9"] = 25;
		registerMap["$k0"] = 26;
		registerMap["$k1"] = 27;
		registerMap["$gp"] = 28;
		registerMap["$sp"] = 29;
		registerMap["$s8"] = 30;
		registerMap["$ra"] = 31;

		constructCommands(file);
		commandCount.assign(commands.size(), 0);
	}

	// perform add operation
	int add(std::string r1, std::string r2, std::string r3)
	{
		return op(r1, r2, r3, [&](int a, int b)
				  { return a + b; });
	}

	// perform subtraction operation
	int sub(std::string r1, std::string r2, std::string r3)
	{
		return op(r1, r2, r3, [&](int a, int b)
				  { return a - b; });
	}

	// perform multiplication operation
	int mul(std::string r1, std::string r2, std::string r3)
	{
		return op(r1, r2, r3, [&](int a, int b)
				  { return a * b; });
	}

	// perform the binary operation
	int op(std::string r1, std::string r2, std::string r3, std::function<int(int, int)> operation)
	{
		if (!checkRegisters({r1, r2, r3}) || registerMap[r1] == 0)
			return 1;
		registers[registerMap[r1]] = operation(registers[registerMap[r2]], registers[registerMap[r3]]);
		PCnext = PCcurr + 1;
		return 0;
	}

	// perform the beq operation
	int beq(std::string r1, std::string r2, std::string label)
	{
		return bOP(r1, r2, label, [](int a, int b)
				   { return a == b; });
	}

	// perform the bne operation
	int bne(std::string r1, std::string r2, std::string label)
	{
		return bOP(r1, r2, label, [](int a, int b)
				   { return a != b; });
	}

	// implements beq and bne by taking the comparator
	int bOP(std::string r1, std::string r2, std::string label, std::function<bool(int, int)> comp)
	{
		if (!checkLabel(label))
			return 4;
		if (address.find(label) == address.end() || address[label] == -1)
			return 2;
		if (!checkRegisters({r1, r2}))
			return 1;
		PCnext = comp(registers[registerMap[r1]], registers[registerMap[r2]]) ? address[label] : PCcurr + 1;
		return 0;
	}

	// implements slt operation
	int slt(std::string r1, std::string r2, std::string r3)
	{
		if (!checkRegisters({r1, r2, r3}) || registerMap[r1] == 0)
			return 1;
		registers[registerMap[r1]] = registers[registerMap[r2]] < registers[registerMap[r3]];
		PCnext = PCcurr + 1;
		return 0;
	}

	// perform the jump operation
	int j(std::string label, std::string unused1 = "", std::string unused2 = "")
	{
		if (!checkLabel(label))
			return 4;
		if (address.find(label) == address.end() || address[label] == -1)
			return 2;
		PCnext = address[label];
		return 0;
	}

	// perform load word operation
	int lw(std::string r, std::string location, std::string unused1 = "")
	{
		if (!checkRegister(r) || registerMap[r] == 0)
			return 1;
		int address = locateAddress(location);
		if (address < 0)
			return abs(address);
		registers[registerMap[r]] = data[address];
		PCnext = PCcurr + 1;
		return 0;
	}

	// perform store word operation
	int sw(std::string r, std::string location, std::string unused1 = "")
	{
		if (!checkRegister(r))
			return 1;
		int address = locateAddress(location);
		if (address < 0)
			return abs(address);
		data[address] = registers[registerMap[r]];
		optdata.push_back(make_pair(address,data[address]));
		PCnext = PCcurr + 1;
		return 0;
	}

	int locateAddress(std::string location)
	{
		if (location.back() == ')')
		{
			try
			{
				int lparen = location.find('('), offset = stoi(lparen == 0 ? "0" : location.substr(0, lparen));
				std::string reg = location.substr(lparen + 1);
				reg.pop_back();
				if (!checkRegister(reg))
					return -3;
				int address = registers[registerMap[reg]] + offset;
				if (address % 4 || address < int(4 * commands.size()) || address >= MAX)
					return -3;
				return address / 4;
			}
			catch (std::exception &e)
			{
				return -4;
			}
		}
		try
		{
			int address = stoi(location);
			if (address % 4 || address < int(4 * commands.size()) || address >= MAX)
				return -3;
			return address / 4;
		}
		catch (std::exception &e)
		{
			return -4;
		}
	}

	// perform add immediate operation
	int addi(std::string r1, std::string r2, std::string num)
	{
		if (!checkRegisters({r1, r2}) || registerMap[r1] == 0)
			return 1;
		try
		{
			registers[registerMap[r1]] = registers[registerMap[r2]] + stoi(num);
			PCnext = PCcurr + 1;
			return 0;
		}
		catch (std::exception &e)
		{
			return 4;
		}
	}

	// checks if label is valid
	inline bool checkLabel(std::string str)
	{
		return str.size() > 0 && isalpha(str[0]) && all_of(++str.begin(), str.end(), [](char c)
														   { return (bool)isalnum(c); }) &&
			   instructions.find(str) == instructions.end();
	}

	// checks if the register is a valid one
	inline bool checkRegister(std::string r)
	{
		return registerMap.find(r) != registerMap.end();
	}

	// checks if all of the registers are valid or not
	bool checkRegisters(std::vector<std::string> regs)
	{
		return std::all_of(regs.begin(), regs.end(), [&](std::string r)
						   { return checkRegister(r); });
	}

	/*
		handle all exit codes:
		0: correct execution
		1: register provided is incorrect
		2: invalid label
		3: unaligned or invalid address
		4: syntax error
		5: commands exceed memory limit
	*/
	void handleExit(exit_code code, int cycleCount)
	{
		switch (code)
		{
		case 1:
			std::cerr << "Invalid register provided or syntax error in providing register\n";
			break;
		case 2:
			std::cerr << "Label used not defined or defined too many times\n";
			break;
		case 3:
			std::cerr << "Unaligned or invalid memory address specified\n";
			break;
		case 4:
			std::cerr << "Syntax error encountered\n";
			break;
		case 5:
			std::cerr << "Memory limit exceeded\n";
			break;
		default:
			break;
		}
		if (code != 0)
		{
			std::cerr << "Error encountered at:\n";
			for (auto &s : commands[PCcurr])
				std::cerr << s << ' ';
			std::cerr << '\n';
		}
		std::cout << "\nFollowing are the non-zero data values:\n";
		for (int i = 0; i < MAX / 4; ++i)
			if (data[i] != 0)
				std::cout << 4 * i << '-' << 4 * i + 3 << std::hex << ": " << data[i] << '\n'
						  << std::dec;
		std::cout << "\nTotal number of cycles: " << cycleCount << '\n';
		std::cout << "Count of instructions executed:\n";
		for (int i = 0; i < (int)commands.size(); ++i)
		{
			std::cout << commandCount[i] << " times:\t";
			for (auto &s : commands[i])
				std::cout << s << ' ';
			std::cout << '\n';
		}
	}

	// parse the command assuming correctly formatted MIPS instruction (or label)
	void parseCommand(std::string line)
	{
		// strip until before the comment begins
		line = line.substr(0, line.find('#'));
		std::vector<std::string> command;
		boost::tokenizer<boost::char_separator<char>> tokens(line, boost::char_separator<char>(", \t"));
		for (auto &s : tokens)
			command.push_back(s);
		// empty line or a comment only line
		if (command.empty())
			return;
		else if (command.size() == 1)
		{
			std::string label = command[0].back() == ':' ? command[0].substr(0, command[0].size() - 1) : "?";
			if (address.find(label) == address.end())
				address[label] = commands.size();
			else
				address[label] = -1;
			command.clear();
		}
		else if (command[0].back() == ':')
		{
			std::string label = command[0].substr(0, command[0].size() - 1);
			if (address.find(label) == address.end())
				address[label] = commands.size();
			else
				address[label] = -1;
			command = std::vector<std::string>(command.begin() + 1, command.end());
		}
		else if (command[0].find(':') != std::string::npos)
		{
			int idx = command[0].find(':');
			std::string label = command[0].substr(0, idx);
			if (address.find(label) == address.end())
				address[label] = commands.size();
			else
				address[label] = -1;
			command[0] = command[0].substr(idx + 1);
		}
		else if (command[1][0] == ':')
		{
			if (address.find(command[0]) == address.end())
				address[command[0]] = commands.size();
			else
				address[command[0]] = -1;
			command[1] = command[1].substr(1);
			if (command[1] == "")
				command.erase(command.begin(), command.begin() + 2);
			else
				command.erase(command.begin(), command.begin() + 1);
		}
		if (command.empty())
			return;
		if (command.size() > 4)
			for (int i = 4; i < (int)command.size(); ++i)
				command[3] += " " + command[i];
		command.resize(4);
		commands.push_back(command);
	}

	// construct the commands vector from the input file
	void constructCommands(std::ifstream &file)
	{
		std::string line;
		while (getline(file, line))
			parseCommand(line);
		file.close();
	}

	void executeCommandsUnpipelined(int bypass){
		if (commands.size() >= MAX / 4)
		{
			handleExit(MEMORY_ERROR, 0);
			return;
		}
		int clockCycles = 0;
		int cc = 0;
		int pc = 0;
		int c =0;
		for (int i = 0; i < 32; ++i){
			register_store[c%5][i] = registers[i];
		}
		for (int i = 0; i < 32; ++i){
			std::cout <<0<<' ';
		}
		std::cout << std::dec << '\n';
		std::cout<< 0 <<'\n';
		c++;
		while (PCcurr < commands.size())
		{
			++clockCycles;
			std::vector<std::string> &command = commands[PCcurr];
			if (instructions.find(command[0]) == instructions.end())
			{
				handleExit(SYNTAX_ERROR, clockCycles);
				return;
			}
			exit_code ret = (exit_code) instructions[command[0]](*this, command[1], command[2], command[3]);
			if (ret != SUCCESS)
			{
				handleExit(ret, clockCycles);
				return;
			}
			++commandCount[PCcurr];
			for (int i = 0; i < 32; ++i){
				register_store[c%5][i] = registers[i];
			}
			c++;
			cc = basic_pipeline(command,cc,pc,bypass);
			PCcurr = PCnext;
		}
		std::vector<string> comm={"no",to_string(c-1)};
		basic_pipeline(comm,cc,pc,bypass);
	}

	void update_under_use(int written){
		for (int i = 0;i<under_use.size();i++){
			if (under_use[i].second == written){
				string finished_register = under_use[i].first;
				under_use.erase(under_use.begin()+i);  // the written register is removed from under_use.
				// the written register is removed from output.
				for (int j =0;j< 2;j++){
					if (output[j][2] == finished_register and outputh[j][0] == written){
						output[j][2] = "";
					}
					if (output[j][3] == finished_register and outputh[j][1] == written){
						output[j][3] = "";
					}
				}
				break;}
		}
	}
	
	void print_registers(int pc){
		for (int i = 0; i < 32; ++i){
			std::cout << register_store[pc%5][i]<<' ';
		}
		std::cout << std::dec << '\n';
	}

	void update_pipeline_5_stage(std::vector<std::string> &command,int cc,int & pc,int toggle ){
		int x = 1;
		if (output[1][2] == "" && output[1][3] == ""){x =0;}
		int stop = 0;
		if (output[2][1].substr(0,2) == "lw" ){
			output[2][1] = output[2][1].substr(2,output[2][1].length()-2);
			int written = stoi(output[2][1]);		// the command which is written this clock cycle.
			// the registers which are written and not under use now are updated in output and under_use data structures.
			if (toggle ==1){update_under_use(written);}
		}
		else if (output[2][1].substr(0,3) == "add"){
			output[2][1] = output[2][1].substr(3,output[2][1].length()-3);
			if (output[3][1] != "0"){
				pc++;
				if (toggle ==0){
					int written = stoi(output[3][1]);
					update_under_use(written); // the registers which are written and not under use now are updated in output and under_use data structures.
				}
			}
			print_registers(pc);
			std::cout<< 1 << ' ';
			std::cout<<optdata[dc].first<<" "<<optdata[dc].second<<'\n';
			dc++;
			stop =1;
		}
		if (output[3][1] != "0" and stop ==0){
			pc++; 
			print_registers(pc);
			std::cout<< 0 <<'\n';
			if (toggle ==0){
				int written = stoi(output[3][1]);
				update_under_use(written); // the registers which are written and not under use now are updated in output and under_use data structures.
			}
		}
		else if (stop ==0){
			print_registers(pc);
			std::cout<<0<<'\n';   
		}
		output[3][1] = output[2][1];
		output[2][1] = "0";
		// the ID stage moves to Ex stage if there is no data hazard.
		if (output[1][2] == "" && output[1][3] == "" && x==0){
			output[2][1] = output[1][1];
			output[1][1] = output[0][1];
			output[1][2] = output[0][2];
			output[1][3] = output[0][3];
			outputh[1][0] = outputh[0][0];
			outputh[1][1] = outputh[0][1];
			output[0][1] = "0";
			output[0][2] = "";
			output[0][3] = "";
			outputh[0][0] = 0;
			outputh[0][1] = 0;
		}
	}

	void update_output(std::vector<std::string> command,int type){
		int c1 = 0;
		int c2 = 0;
		if (type ==1){
			for (int i = under_use.size()-1;i >=0;i--){
				if (output[0][2] == ""){
					if (under_use[i].first == command[0] and c1 ==0){
						output[0][2] = command[0];
						outputh[0][0] = under_use[i].second;
						c1 =1;
					}
				}
				else {break;}
			}
		}
		else if (type ==2){
			for (int i = under_use.size() -1;i >=0;i--){
				if (output[0][2] == ""){
					if (under_use[i].first == command[0] and c1 ==0){
						output[0][2] = command[0];
						outputh[0][0] = under_use[i].second;
						c1 =1;
					}
					else if (under_use[i].first == command[1] and c2 ==0){
						output[0][2] = command[1];
						outputh[0][0] = under_use[i].second;
						c2 =1;
					}
				}
				else if (output[0][3] ==""){
					if (under_use[i].first == command[0] and c1 ==0){
						output[0][3] = command[0];
						outputh[0][1] = under_use[i].second;
						c1 =1;
					}
					else if (under_use[i].first == command[1] and c2 ==0){
						output[0][3] = command[1];
						outputh[0][1] = under_use[i].second;
						c2 =1;
					}
				}
				else {break;}
			}
		}
	}

	string find_register(string comm){
		for (int i = 0;i<comm.size();i++){
			if (comm[i] =='('){
				return comm.substr(i+1,comm.size()-i-2);
			}
		}
	}

	int basic_pipeline(std::vector<std::string> &command,int cc,int & pc,int bypass){
		update_pipeline_5_stage(command,cc,pc,bypass);
		cc++;
		if (output[0][1] != "0"){
			return basic_pipeline(command,cc,pc,bypass);
		}
		else if (command[0] == "no"){
			int k = stoi(command[1]);
			while (pc<k){
				update_pipeline_5_stage(command,cc,pc,bypass);
				cc++;
			}
		}
		else if (command[0] == "add" or  command[0] == "mul" or command[0] == "sub"){
			output[0][1] = to_string(cc);
			update_output({command[2],command[3]},2);
			if (bypass == 0){under_use.push_back(make_pair(command[1],cc));}		
		}
		else if (command[0] == "addi"){
			output[0][1] = to_string(cc);
			update_output({command[2]},1);
			if (bypass == 0){under_use.push_back(make_pair(command[1],cc));}
		}
		else if (command[0] == "j"){
			output[0][1] = to_string(cc);
			update_pipeline_5_stage(command,cc,pc,bypass);
			cc++;
		}
		else if (command[0] == "beq" or command[0] == "bne" or command[0] == "slt"){
			output[0][1] = to_string(cc);
			string k = to_string(cc);
			update_output({command[1],command[2]},2);
			while(output[2][1] != k){
				update_pipeline_5_stage(command,cc,pc,bypass);
				cc++;
			}	
		}
		else if (command[0] == "lw"){
			string comm = find_register(command[2]);
			output[0][1] = "lw" + to_string(cc);
			update_output({comm},1);
			under_use.push_back(make_pair(command[1],cc));
		}
		else if (command[0] == "sw"){
			string comm = find_register(command[2]);
			output[0][1] = "add" +to_string(cc);
			update_output({comm,command[1]},2);
		}
		return cc;
	}
};
#endif