#include <core/cfgparser.h>

enum ParserState {
	ParserStateHeading,
	ParserStateName,
	ParserStateValue,
};

CFGParser::CFGParser(const char* path){
	cfgFile = fopen(path, "rw");

	if(!cfgFile){
		printf("CFGParser: Failed to open %s!", path);
	}
}

CFGParser::~CFGParser(){
	if(cfgFile){
		fclose(cfgFile);
	}
}

void CFGParser::Parse(){
	int c;
	int state = ParserStateName;

	std::string headingName;
	std::string name;
	std::string value;

	std::vector<CFGItem> values;
	while((c = fgetc(cfgFile)) != EOF){
		switch(c){
		case '=':
			if(state == ParserStateName){
				state = ParserStateValue;
				break;
			} else {
				value += c;
				break;
			}
		case '#':
			while((c = fgetc(cfgFile)) != EOF && c != '\n');
		case '\n':
			if(state == ParserStateValue){
				CFGItem item;

				item.name = name;
				item.value = value;

				values.push_back(item);
			} else if(state == ParserStateName) {
				if(!name.length()){
					break; // Empty line?
				}

				CFGItem item;
				item.name = "";
				item.value = name;
			} else if(state == ParserStateHeading){
				printf("CFGParser: Potentially malformed heading [%s\n", headingName);
				headingName.clear();
			}

			name.clear();
			value.clear();

			state = ParserStateName;
			break;
		case ']':
		case '[':
			if(c == '[' && state == ParserStateName && !name.length()){
				state = ParserStateHeading;
				if(values.size() > 0){ // Don't add empty headings
					items.push_back(std::pair<std::string, std::vector<CFGItem>>(headingName, values));
				}
				headingName.clear();
				values.clear();
				break;
			} else if(c == ']' && state == ParserStateHeading){
				state = ParserStateName;
				break;
			}
		default:
			if(state == ParserStateHeading){
				headingName += c;
			} else if(state == ParserStateName){
				name += c;
			} else if(state == ParserStateValue){
				value += c;
			}
			break;
		}
	}

	if(state == ParserStateValue){
		CFGItem item;

		item.name = name;
		item.value = value;

		values.push_back(item);
	}

	items.push_back(std::pair<std::string, std::vector<CFGItem>>(headingName, values));
}