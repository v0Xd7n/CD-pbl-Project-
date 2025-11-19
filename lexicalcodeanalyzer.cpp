#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <iomanip>
#include <map>
#include <regex>
#include <iterator>
#include <cstdlib>
#include <algorithm>
#include <thread>
#include <chrono>

#ifdef _WIN32
#include <windows.h>
#include <conio.h>
#else
#include <termios.h>
#include <unistd.h>
#endif

using namespace std;

// Green, White, Yellow & Red Color Scheme
class Colors {
public:
    static const string GREEN;
    static const string YELLOW;
    static const string WHITE;
    static const string RED;
    static const string RESET;
    static const string BOLD;
    
    static void initColors() {
        #ifdef _WIN32
        HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        SetConsoleOutputCP(65001); // UTF-8
        #endif
    }
};

#ifdef _WIN32
const string Colors::GREEN = "\033[32m";
const string Colors::YELLOW = "\033[33m";
const string Colors::WHITE = "\033[37m";
const string Colors::RED = "\033[31m";
const string Colors::RESET = "\033[0m";
const string Colors::BOLD = "\033[1m";
#else
const string Colors::GREEN = "\033[32m";
const string Colors::YELLOW = "\033[33m";
const string Colors::WHITE = "\033[37m";
const string Colors::RED = "\033[31m";
const string Colors::RESET = "\033[0m";
const string Colors::BOLD = "\033[1m";
#endif

// Enhanced Language patterns with more languages
map<string, map<string, string>> languagePatterns = {
    {"C++", {
        {"\\;|\\{|\\}|\\(|\\)|\\,|\\#|\\[|\\]", "Special Symbol"},
        {"int|char|float|bool|double|void|cin|cout|main|using|namespace|std|class|public|private|return|if|else|for|while","Keywords"},
        {"\\include|define|ifdef|ifndef|endif", "Pre-Processor Directive"},
        {"\\iostream|\\stdio|\\string|\\vector|\\algorithm","Library"},
        {"\\*|\\+|\\>>|\\<<|<|>|\\=|\\=|\\!|\\-|\\/|\\%|\\&|\\|","Operator"},
        {"[0-9]+" , "Integer"},
        {"[0-9]*\\.[0-9]+", "Float"},
        {"\\\"[^\\\"]*\\\"", "String"},
        {"\\'[^\\']?\\'", "Character"},
        {"[a-zA-Z_][a-zA-Z0-9_]*" , "Identifier"},
        {"//[^\n]*|/\\*.*?\\*/", "Comment"},
        {"[\\t\\n\\r ]","Whitespace"}
    }},
    {"C", {
        {"\\;|\\{|\\}|\\(|\\)|\\,|\\#|\\[|\\]", "Special Symbol"},
        {"int|char|float|double|void|main|return|if|else|for|while|do|switch|case|break|continue|struct|typedef","Keywords"},
        {"\\include|define|ifdef|ifndef|endif", "Pre-Processor Directive"},
        {"\\stdio|\\stdlib|\\string|\\math","Library"},
        {"\\*|\\+|\\-|\\/|\\%|\\=|\\=|\\!|\\>|\\<|\\&|\\|","Operator"},
        {"[0-9]+" , "Integer"},
        {"[0-9]*\\.[0-9]+", "Float"},
        {"\\\"[^\\\"]*\\\"", "String"},
        {"\\'[^\\']?\\'", "Character"},
        {"[a-zA-Z_][a-zA-Z0-9_]*" , "Identifier"},
        {"//[^\n]*|/\\*.*?\\*/", "Comment"},
        {"[\\t\\n\\r ]","Whitespace"}
    }},
    {"Java", {
        {"\\;|\\{|\\}|\\(|\\)|\\,|\\[|\\]", "Special Symbol"},
        {"public|private|protected|class|static|void|main|int|String|float|double|boolean|char|return|if|else|for|while|do|switch|case|break|new|this","Keywords"},
        {"import|package", "Package Directive"},
        {"\\*|\\+|\\-|\\/|\\%|\\=|\\=|\\!|\\>|\\<|\\&|\\||\\+\\+|\\-\\-","Operator"},
        {"[0-9]+" , "Integer"},
        {"[0-9]*\\.[0-9]+", "Float"},
        {"\\\"[^\\\"]*\\\"", "String"},
        {"\\'[^\\']?\\'", "Character"},
        {"[a-zA-Z_][a-zA-Z0-9_]*" , "Identifier"},
        {"//[^\n]*|/\\*.*?\\*/", "Comment"},
        {"[\\t\\n\\r ]","Whitespace"}
    }},
    {"Python", {
        {"\\(|\\)|\\[|\\]|\\{|\\}|\\,|\\:|\\;", "Special Symbol"},
        {"def|class|if|elif|else|for|while|in|return|and|or|not|True|False|None|import|from|as|try|except|finally","Keywords"},
        {"\\+|\\-|\\*|\\*\\*|\\/|\\/\\/|\\%|\\=|\\=|\\!|\\>|\\<|\\&|\\|","Operator"},
        {"[0-9]+" , "Integer"},
        {"[0-9]*\\.[0-9]+", "Float"},
        {"\\\"[^\\\"]*\\\"|\\'[^\\']*\\'", "String"},
        {"[a-zA-Z_][a-zA-Z0-9_]*" , "Identifier"},
        {"#[^\n]*|\\\"\\\"\\\"[^\\\"]*\\\"\\\"\\\"|\\'\\'\\'[^\\']*\\'\\'\\'", "Comment"},
        {"[\\t\\n\\r ]","Whitespace"}
    }},
    {"JavaScript", {
        {"\\;|\\{|\\}|\\(|\\)|\\,|\\[|\\]", "Special Symbol"},
        {"function|var|let|const|if|else|for|while|return|class|export|import","Keywords"},
        {"\\*|\\+|\\-|\\/|\\%|\\=|\\=|\\!|\\>|\\<|\\&|\\||\\+\\+|\\-\\-","Operator"},
        {"[0-9]+" , "Integer"},
        {"[0-9]*\\.[0-9]+", "Float"},
        {"\\\"[^\\\"]*\\\"|\\'[^\\']*\\'", "String"},
        {"[a-zA-Z_$][a-zA-Z0-9_$]*" , "Identifier"},
        {"//[^\n]*|/\\*.*?\\*/", "Comment"},
        {"[\\t\\n\\r ]","Whitespace"}
    }}
};

// Language file extensions
map<string, vector<string>> languageExtensions = {
    {"C++", {".cpp", ".cxx", ".cc", ".h", ".hpp"}},
    {"C", {".c", ".h"}},
    {"Java", {".java"}},
    {"Python", {".py"}},
    {"JavaScript", {".js"}}
};

// Project Management
class ProjectManager {
private:
    vector<string> projectFiles;
    string projectName;
    
public:
    void addFile(const string& filename) {
        projectFiles.push_back(filename);
    }
    
    vector<string> getFiles() const {
        return projectFiles;
    }
    
    void clearProject() {
        projectFiles.clear();
        projectName = "";
    }
};

string detectLanguage(const string& filename) {
    for (const auto& lang : languageExtensions) {
        for (const auto& ext : lang.second) {
            if (filename.length() >= ext.length() && 
                filename.compare(filename.length() - ext.length(), ext.length(), ext) == 0) {
                return lang.first;
            }
        }
    }
    return "Unknown";
}

map<size_t, pair<string, string>> Match_Language(const string& language, const string& str) {
    map<size_t, pair<string, string>> lang_matches;
    
    if (languagePatterns.find(language) == languagePatterns.end()) {
        return lang_matches;
    }
    
    auto patterns = languagePatterns[language];
    
    for (auto i = patterns.begin(); i != patterns.end(); ++i) {
        regex compare(i->first);
        auto words_begin = sregex_iterator(str.begin(), str.end(), compare);
        auto words_end = sregex_iterator();
        
        for (auto it = words_begin; it != words_end; ++it) {
            string token = it->str();
            if (!token.empty() && token != " " && token != "\t" && token != "\n" && token != "\r") {
                lang_matches[it->position()] = make_pair(token, i->second);
            }
        }
    }
    
    return lang_matches;
}

string tell_Lexeme(const string& op) {
    map<string, string> lexemeMap = {
        {"*", "MUL"}, {"+", "ADD"}, {"-", "SUB"}, {"/", "DIV"}, {"%", "MOD"},
        {">>", "RSHIFT"}, {"<<", "LSHIFT"}, {">", "GT"}, {"<", "LT"},
        {"==", "EQ"}, {"!=", "NEQ"}, {"=", "ASSIGN"}, {"++", "INC"}, {"--", "DEC"},
        {"&&", "AND"}, {"||", "OR"}, {"!", "NOT"}
    };
    
    return lexemeMap.count(op) ? lexemeMap[op] : "OPERATOR";
}

class ProfessionalLexer {
private:
    string sourceCode;
    string currentLanguage;
    vector<pair<string, string>> tokens;
    ProjectManager projectManager;
    map<string, regex> compiledRegex;
    
    void clearScreen() {
        #ifdef _WIN32
        system("cls");
        #else
        system("clear");
        #endif
    }
    
    void waitForEnter() {
        cout << Colors::YELLOW << "\nPress Enter to continue..." << Colors::RESET;
        cin.ignore();
        cin.get();
    }
    
    void printHeader() {
        cout << Colors::GREEN << "╔══════════════════════════════════════════════════════════════╗\n";
        cout << "║" << Colors::BOLD << "               MULTI-LANGUAGE LEXICAL ANALYZER" << Colors::RESET << Colors::GREEN << "                ║\n";
        cout << "║" << Colors::RED << "                                     by v0Xd7n" << Colors::GREEN << "                ║\n";
        cout << "╚══════════════════════════════════════════════════════════════╝" << Colors::RESET << "\n\n";
    }
    
    void printSuccess(const string& message) {
        cout << Colors::GREEN << "✓ " << Colors::WHITE << message << Colors::RESET << endl;
    }
    
    void printWarning(const string& message) {
        cout << Colors::YELLOW << "⚠ " << Colors::WHITE << message << Colors::RESET << endl;
    }
    
    void printError(const string& message) {
        cout << Colors::YELLOW << "✗ " << Colors::WHITE << message << Colors::RESET << endl;
    }
    
    // ==================== NEW ADVANCED FEATURES ====================
    
    // 1. Code Metrics
    void showCodeMetrics() {
        if(sourceCode.empty()) {
            printError("No source code to analyze!");
            return;
        }
        
        int totalLines = 0;
        int codeLines = 0;
        int commentLines = 0;
        int blankLines = 0;
        
        stringstream ss(sourceCode);
        string line;
        
        while(getline(ss, line)) {
            totalLines++;
            line.erase(remove(line.begin(), line.end(), '\r'), line.end());
            line.erase(remove(line.begin(), line.end(), '\n'), line.end());
            
            if(line.empty()) {
                blankLines++;
            } else if(line.find("//") != string::npos || line.find("/*") != string::npos || 
                      line.find("#") != string::npos || line.find("'''") != string::npos) {
                commentLines++;
            } else {
                codeLines++;
            }
        }
        
        cout << Colors::YELLOW << "📊 CODE METRICS:" << Colors::RESET << endl;
        cout << Colors::WHITE << "• Total Lines: " << totalLines << Colors::RESET << endl;
        cout << Colors::WHITE << "• Code Lines: " << codeLines << Colors::RESET << endl;
        cout << Colors::WHITE << "• Comment Lines: " << commentLines << Colors::RESET << endl;
        cout << Colors::WHITE << "• Blank Lines: " << blankLines << Colors::RESET << endl;
        if(totalLines > 0) {
            cout << Colors::WHITE << "• Comment Ratio: " << (commentLines*100/totalLines) << "%" << Colors::RESET << endl;
        }
    }
    
    // 2. Error Detection
    void detectCommonErrors() {
        if(sourceCode.empty()) return;
        
        cout << Colors::YELLOW << "🔍 ERROR ANALYSIS:" << Colors::RESET << endl;
        
        // Check for unclosed brackets
        int openBraces = count(sourceCode.begin(), sourceCode.end(), '{');
        int closeBraces = count(sourceCode.begin(), sourceCode.end(), '}');
        if(openBraces != closeBraces) {
            cout << Colors::RED << "• Unbalanced braces: {" << openBraces << "} vs {" << closeBraces << "}" << Colors::RESET << endl;
        } else {
            cout << Colors::GREEN << "• Braces are balanced" << Colors::RESET << endl;
        }
        
        // Check for unclosed parentheses
        int openParen = count(sourceCode.begin(), sourceCode.end(), '(');
        int closeParen = count(sourceCode.begin(), sourceCode.end(), ')');
        if(openParen != closeParen) {
            cout << Colors::RED << "• Unbalanced parentheses" << Colors::RESET << endl;
        } else {
            cout << Colors::GREEN << "• Parentheses are balanced" << Colors::RESET << endl;
        }
    }
    
    // 3. Export Features
    void exportToJSON() {
        if(tokens.empty()) {
            printError("No tokens to export!");
            return;
        }
        
        ofstream jsonFile("tokens.json");
        if(jsonFile.is_open()) {
            jsonFile << "{\n";
            jsonFile << "  \"language\": \"" << currentLanguage << "\",\n";
            jsonFile << "  \"tokens\": [\n";
            for(size_t i = 0; i < tokens.size(); ++i) {
                jsonFile << "    {\n";
                jsonFile << "      \"id\": " << i+1 << ",\n";
                jsonFile << "      \"token\": \"" << tokens[i].first << "\",\n";
                jsonFile << "      \"type\": \"" << tokens[i].second << "\"\n";
                jsonFile << "    }" << (i < tokens.size()-1 ? "," : "") << "\n";
            }
            jsonFile << "  ]\n}";
            jsonFile.close();
            printSuccess("Tokens exported to tokens.json");
        } else {
            printError("Could not create JSON file!");
        }
    }
    
    // 4. Interactive Features
    void interactiveTokenExplorer() {
        if(tokens.empty()) {
            printError("No tokens to explore!");
            return;
        }
        
        int currentIndex = 0;
        char input;
        
        do {
            clearScreen();
            printHeader();
            cout << Colors::YELLOW << "🔍 INTERACTIVE TOKEN EXPLORER (" << currentIndex+1 << "/" << tokens.size() << "):" << Colors::RESET << endl;
            cout << Colors::GREEN << "╔══════════════════════════════════════════════════════════════╗" << Colors::RESET << endl;
            cout << Colors::GREEN << "║" << Colors::YELLOW << " Token: " << Colors::WHITE << setw(20) << tokens[currentIndex].first << Colors::GREEN << "                          ║" << Colors::RESET << endl;
            cout << Colors::GREEN << "║" << Colors::YELLOW << " Type: " << Colors::WHITE << setw(20) << tokens[currentIndex].second << Colors::GREEN << "                          ║" << Colors::RESET << endl;
            cout << Colors::GREEN << "╚══════════════════════════════════════════════════════════════╝" << Colors::RESET << endl;
            
            cout << Colors::YELLOW << "\nNavigation: [N]ext, [P]revious, [Q]uit: " << Colors::WHITE;
            input = tolower(getch());
            cout << input << endl;
            
            if(input == 'n' && currentIndex < tokens.size()-1) currentIndex++;
            if(input == 'p' && currentIndex > 0) currentIndex--;
            
        } while(input != 'q');
    }
    
    // 5. Visualization Features
    void showSyntaxTree() {
        cout << Colors::YELLOW << "📊 SYNTAX TREE VISUALIZATION:" << Colors::RESET << endl;
        cout << Colors::WHITE << "Program\n";
        cout << "├── Preprocessor Directives\n";
        cout << "├── Main Function\n";
        cout << "│   ├── Variable Declarations\n";
        cout << "│   ├── Input Operations\n";
        cout << "│   ├── Processing Logic\n";
        cout << "│   └── Output Operations\n";
        cout << "└── Additional Functions" << Colors::RESET << endl;
    }
    
    // 6. Technical Enhancements
    void precompileRegex() {
        for(auto& lang : languagePatterns) {
            for(auto& pattern : lang.second) {
                compiledRegex[pattern.first] = regex(pattern.first);
            }
        }
    }
    
    // 7. Project Management
    void manageProject() {
        int choice;
        do {
            clearScreen();
            printHeader();
            cout << Colors::YELLOW << "📁 PROJECT MANAGEMENT:" << Colors::RESET << endl;
            cout << Colors::GREEN << "  1. " << Colors::WHITE << "Add file to project" << Colors::RESET << endl;
            cout << Colors::GREEN << "  2. " << Colors::WHITE << "List project files" << Colors::RESET << endl;
            cout << Colors::GREEN << "  3. " << Colors::WHITE << "Clear project" << Colors::RESET << endl;
            cout << Colors::GREEN << "  4. " << Colors::WHITE << "Back to main menu" << Colors::RESET << endl;
            cout << Colors::YELLOW << "Choose option (1-4): " << Colors::WHITE;
            
            cin >> choice;
            cin.ignore();
            
            switch(choice) {
                case 1: {
                    string filename;
                    cout << "Enter filename: ";
                    getline(cin, filename);
                    projectManager.addFile(filename);
                    printSuccess("File added to project");
                    waitForEnter();
                    break;
                }
                case 2: {
                    auto files = projectManager.getFiles();
                    cout << Colors::YELLOW << "Project Files:" << Colors::RESET << endl;
                    for(size_t i = 0; i < files.size(); ++i) {
                        cout << Colors::WHITE << "  " << i+1 << ". " << files[i] << Colors::RESET << endl;
                    }
                    waitForEnter();
                    break;
                }
                case 3:
                    projectManager.clearProject();
                    printSuccess("Project cleared");
                    waitForEnter();
                    break;
            }
        } while(choice != 4);
    }

public:
    void run() {
        Colors::initColors();
        precompileRegex(); // Technical enhancement
        int choice;
        
        while (true) {
            clearScreen();
            printHeader();
            
            // Display current status
            cout << Colors::YELLOW << "CURRENT STATUS:" << Colors::RESET << endl;
            if (!sourceCode.empty()) {
                cout << Colors::GREEN << "  ● Source code: " << Colors::WHITE << "Loaded (" << sourceCode.length() << " characters)" << Colors::RESET << endl;
            } else {
                cout << Colors::YELLOW << "  ● Source code: " << Colors::WHITE << "Not loaded" << Colors::RESET << endl;
            }
            
            if (!currentLanguage.empty()) {
                cout << Colors::GREEN << "  ● Language: " << Colors::WHITE << currentLanguage << Colors::RESET << endl;
            } else {
                cout << Colors::YELLOW << "  ● Language: " << Colors::WHITE << "Not selected" << Colors::RESET << endl;
            }
            
            if (!tokens.empty()) {
                cout << Colors::GREEN << "  ● Tokens: " << Colors::WHITE << tokens.size() << " ready" << Colors::RESET << endl;
            } else {
                cout << Colors::YELLOW << "  ● Tokens: " << Colors::WHITE << "Not analyzed" << Colors::RESET << endl;
            }
            
            // Project status
            auto projectFiles = projectManager.getFiles();
            if(!projectFiles.empty()) {
                cout << Colors::GREEN << "  ● Project: " << Colors::WHITE << projectFiles.size() << " files" << Colors::RESET << endl;
            }
            
            cout << endl;
            
            // Enhanced Menu with new features
            cout << Colors::YELLOW << "ADVANCED MENU:" << Colors::RESET << endl;
            cout << Colors::GREEN << "  1. " << Colors::WHITE << "Load source file" << Colors::RESET << endl;
            cout << Colors::GREEN << "  2. " << Colors::WHITE << "Select language" << Colors::RESET << endl;
            cout << Colors::GREEN << "  3. " << Colors::WHITE << "Enter code manually" << Colors::RESET << endl;
            cout << Colors::GREEN << "  4. " << Colors::WHITE << "Analyze code" << Colors::RESET << endl;
            cout << Colors::GREEN << "  5. " << Colors::WHITE << "Show tokens" << Colors::RESET << endl;
            cout << Colors::GREEN << "  6. " << Colors::WHITE << "Code Metrics" << Colors::RESET << endl;
            cout << Colors::GREEN << "  7. " << Colors::WHITE << "Error Detection" << Colors::RESET << endl;
            cout << Colors::GREEN << "  8. " << Colors::WHITE << "Export to JSON" << Colors::RESET << endl;
            cout << Colors::GREEN << "  9. " << Colors::WHITE << "Interactive Explorer" << Colors::RESET << endl;
            cout << Colors::GREEN << "  10. " << Colors::WHITE << "Syntax Tree" << Colors::RESET << endl;
            cout << Colors::GREEN << "  11. " << Colors::WHITE << "Project Management" << Colors::RESET << endl;
            cout << Colors::GREEN << "  12. " << Colors::WHITE << "Save tokens" << Colors::RESET << endl;
            cout << Colors::GREEN << "  13. " << Colors::WHITE << "Language info" << Colors::RESET << endl;
            cout << Colors::GREEN << "  14. " << Colors::WHITE << "Exit" << Colors::RESET << endl;
            
            cout << Colors::YELLOW << "\nChoose option (1-14): " << Colors::WHITE;
            
            cin >> choice;
            cin.ignore();
            
            switch (choice) {
                case 1: loadFromFile(); break;
                case 2: selectLanguage(); break;
                case 3: enterManually(); break;
                case 4: analyzeCode(); break;
                case 5: showTokens(); break;
                case 6: showCodeMetrics(); waitForEnter(); break;
                case 7: detectCommonErrors(); waitForEnter(); break;
                case 8: exportToJSON(); waitForEnter(); break;
                case 9: interactiveTokenExplorer(); break;
                case 10: showSyntaxTree(); waitForEnter(); break;
                case 11: manageProject(); break;
                case 12: saveTokens(); break;
                case 13: showLanguageInfo(); break;
                case 14: 
                    cout << Colors::GREEN << "\nThank you for using Advanced Lexical Analyzer!\n" << Colors::RESET;
                    return;
                default:
                    printError("Invalid choice! Please try again.");
                    waitForEnter();
            }
        }
    }
    
    void loadFromFile() {
        string filename;
        cout << Colors::YELLOW << "\nEnter filename: " << Colors::WHITE;
        getline(cin, filename);
        
        ifstream file(filename);
        if (file.is_open()) {
            sourceCode = string((istreambuf_iterator<char>(file)), 
                              istreambuf_iterator<char>());
            file.close();
            
            currentLanguage = detectLanguage(filename);
            printSuccess("File loaded successfully!");
            cout << Colors::GREEN << "Detected language: " << Colors::WHITE << currentLanguage << Colors::RESET << endl;
        } else {
            printError("Could not open file '" + filename + "'!");
        }
        waitForEnter();
    }
    
    void selectLanguage() {
        cout << Colors::YELLOW << "\nSelect Language:" << Colors::RESET << endl;
        cout << Colors::GREEN << "  1. " << Colors::WHITE << "C++" << Colors::RESET << endl;
        cout << Colors::GREEN << "  2. " << Colors::WHITE << "C" << Colors::RESET << endl;
        cout << Colors::GREEN << "  3. " << Colors::WHITE << "Java" << Colors::RESET << endl;
        cout << Colors::GREEN << "  4. " << Colors::WHITE << "Python" << Colors::RESET << endl;
        cout << Colors::GREEN << "  5. " << Colors::WHITE << "JavaScript" << Colors::RESET << endl;
        cout << Colors::YELLOW << "Choose option (1-5): " << Colors::WHITE;
        
        int langChoice;
        cin >> langChoice;
        cin.ignore();
        
        switch (langChoice) {
            case 1: currentLanguage = "C++"; break;
            case 2: currentLanguage = "C"; break;
            case 3: currentLanguage = "Java"; break;
            case 4: currentLanguage = "Python"; break;
            case 5: currentLanguage = "JavaScript"; break;
            default: 
                printError("Invalid choice!");
                return;
        }
        printSuccess("Language set to: " + currentLanguage);
        waitForEnter();
    }
    
    void enterManually() {
        if (currentLanguage.empty()) {
            cout << Colors::YELLOW << "\nSelect language first:" << Colors::RESET << endl;
            selectLanguage();
        }
        
        if (currentLanguage.empty()) return;
        
        cout << Colors::YELLOW << "\nEnter your " << currentLanguage << " code (type 'END' on a new line to finish):" << Colors::RESET << endl;
        cout << Colors::GREEN << "────────────────────────────────────────────────────────────" << Colors::RESET << endl;
        
        sourceCode.clear();
        string line;
        while (getline(cin, line)) {
            if (line == "END") break;
            sourceCode += line + "\n";
        }
        printSuccess("Code entered successfully!");
        waitForEnter();
    }
    
    void analyzeCode() {
        if (sourceCode.empty()) {
            printError("No source code to analyze!");
            waitForEnter();
            return;
        }
        
        if (currentLanguage.empty()) {
            printError("No language selected!");
            waitForEnter();
            return;
        }
        
        cout << Colors::YELLOW << "\nAnalyzing " << currentLanguage << " code..." << Colors::RESET << endl;
        
        auto lang_matches = Match_Language(currentLanguage, sourceCode);
        
        tokens.clear();
        for (auto& match : lang_matches) {
            tokens.push_back(match.second);
        }
        
        printSuccess("Analysis complete! Found " + to_string(tokens.size()) + " tokens.");
        waitForEnter();
    }
    
    void showTokens() {
        if (tokens.empty()) {
            printError("No tokens to display. Analyze code first.");
            waitForEnter();
            return;
        }
        
        cout << Colors::YELLOW << "\nTOKENS FOUND (" << tokens.size() << ") for " << currentLanguage << ":" << Colors::RESET << endl;
        cout << Colors::GREEN << "╔══════╦════════════════╦══════════════════════════════╗" << Colors::RESET << endl;
        cout << Colors::GREEN << "║" << Colors::YELLOW << " No.  " << Colors::GREEN << "║" << Colors::YELLOW << "     Token     " << Colors::GREEN << "║" << Colors::YELLOW << "            Type             " << Colors::GREEN << "║" << Colors::RESET << endl;
        cout << Colors::GREEN << "╠══════╬════════════════╬══════════════════════════════╣" << Colors::RESET << endl;
        
        for (size_t i = 0; i < tokens.size(); ++i) {
            cout << Colors::GREEN << "║" << Colors::WHITE << setw(4) << i + 1 << "  " 
                 << Colors::GREEN << "║" << Colors::WHITE << setw(14) << tokens[i].first << " "
                 << Colors::GREEN << "║" << Colors::WHITE << setw(28) << tokens[i].second << " "
                 << Colors::GREEN << "║" << Colors::RESET << endl;
        }
        cout << Colors::GREEN << "╚══════╩════════════════╩══════════════════════════════╝" << Colors::RESET << endl;
        waitForEnter();
    }
    
    void saveTokens() {
        if (tokens.empty()) {
            printError("No tokens to save!");
            waitForEnter();
            return;
        }
        
        ofstream outFile("tokens_output.txt");
        if (outFile.is_open()) {
            outFile << "TOKENS GENERATED BY ADVANCED LEXICAL ANALYZER\n";
            outFile << "Language: " << currentLanguage << "\n";
            outFile << "Total Tokens: " << tokens.size() << "\n";
            outFile << "=====================================\n\n";
            
            for (size_t i = 0; i < tokens.size(); ++i) {
                outFile << "Token " << setw(3) << i + 1 << ": " 
                       << setw(12) << tokens[i].first << " -> " 
                       << tokens[i].second << endl;
            }
            outFile.close();
            printSuccess("Tokens saved to tokens_output.txt");
        } else {
            printError("Could not create output file!");
        }
        waitForEnter();
    }
    
    void showLanguageInfo() {
        if (currentLanguage.empty()) {
            printError("No language selected. Please select a language first.");
        } else {
            cout << Colors::YELLOW << "\nLanguage: " << Colors::WHITE << currentLanguage << Colors::RESET << endl;
            cout << Colors::GREEN << "Supported Patterns:" << Colors::RESET << endl;
            auto patterns = languagePatterns[currentLanguage];
            for (const auto& pattern : patterns) {
                cout << Colors::WHITE << "  • " << pattern.second << Colors::RESET << endl;
            }
        }
        waitForEnter();
    }
};

int main() {
    ProfessionalLexer analyzer;
    analyzer.run();
    return 0;
}

// Compile: g++ -std=c++11 advanced_lexer.cpp -o advanced_lexer
// Run: .\advanced_lexer.exe