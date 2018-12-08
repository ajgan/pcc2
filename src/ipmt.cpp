using namespace std;
#include <iostream>
#include <stdio.h>
#include <cstring>
#include <string>
#include <iostream>
#include <fstream>
#include <math.h>
#include <bitset>
#include <vector>

struct suffix  {
    int index;
    char *suf;
};

int compareSuffix(struct suffix a, struct suffix b) {
    return strcmp(a.suf, b.suf) < 0? 1 : 0;
}

int* suffixArray(char* txt) {
    int n = strlen(txt);
    struct suffix suffixes[n];

    for (int i = 0; i < n; i++) {
        suffixes[i].index = i;
        suffixes[i].suf = (txt+i);
    }

    sort(suffixes, suffixes+n, compareSuffix);

    int *suffixArr = new int[n];
    for (int i = 0; i < n; i++) {
        suffixArr[i] = suffixes[i].index;
    }

    return  suffixArr;
}

string encodeBits(int size, int n) {
    bool power2 = ((n & (n - 1)) == 0);
    int nbits = ceil(log2(size));
    if(power2) nbits++;
    int a[nbits];
    for (int i = 0; i < nbits; i++) {
        a[i] = 0;
    }
    string s;
    for(int i=0; n>0; i++) {
        a[i] = n%2;
        n = n/2;
    }
    for(int i=nbits-1 ;i>=0 ;i--) {
        s+=to_string(a[i]);
    }
    return s;
}

unsigned char decodeBits(string bits) {
    int result = 0;
    char pos;
    for (int a = 0; a < 8; a++) {
        pos = (int)bits[7-a];
        result += ((pos-48) * pow(2,a));
    }
    unsigned char test = (unsigned char)result;
    // cout << test << (int)test << "\n";
    // cout << (unsigned char)result << "\n";
    return test;
}

string lz77(string txt, int window, int buff) {

    string w;
    for (int a = 0; a < window; a++) {
        w += char(0);
    }
    txt = w + txt;
    string code = "";
    string code2 = "";
    string pref;
    string windPref;
    int pos;
    int len;
    char letter;
    bool match;
    int n = txt.length();

    for (int i = window; i < n; i+=(len+1)) {
        pref = "";
        windPref = "";
        pos = 0;
        len = 0;
        // W[j-ls:min(n,j+ll)],W[j:min(n,j+ll)]
        for (int j = 0; j < buff; j++) {
            pref = txt.substr(i, j+1);
            match = 0;
            // 'window -j' não invade  -> 'window' invade lookahead buffer
            for (int k = 0; k < window - j ; k++) {
                windPref = txt.substr(i-window+k, j+1);
                if (pref == windPref) {
                    match = 1;
                    pos = window - k;
                    len = j + 1;
                    break;
                }
            }
            if(match == 0) break;

        }
        try {
            letter = txt[i+len];
        }
        catch (int e) {
            letter = char(3);
        }
        string result = to_string(pos) + to_string(len) + (letter);
        string result2 = encodeBits(window, pos) + encodeBits(buff, len) + encodeBits(255, int(letter));
        code += result;
        code2 += result2;
    }
    cout << code << "\n";
    return code2;
}

string compressIndexArray (int *sufArr, int len) {
    string result = "";
    for (int i = 0; i < len; i++) {
        result += encodeBits(len - 1, sufArr[i]);
    }
    return result;
}

void showhelp() {
    string my_val = "Formato de comando:\n"
    "./ipmt index textfile || ./ipmt search [options] pattern indexfile\n"
    "Flags de opção do modo de busca(search):\n"
    "-c ou --count\n"
    "    Exibe contagem de ocorrências do padrão no texto(por default, não se exibe)\n"
    "-p ou --pattern patternfile\n"
    "     Indica que os padrões devem vir de um arquivo patternfile\n"
    "-h ou --help\n"
    "     Exibe informações básicas da ferramenta\n";
    cout << my_val << "\n";
}

int main(int argc, char** argv) {

    int markedPos[argc];
    for (int a = 0; a < argc; a++) {
        markedPos[a] = 0;
    }
    markedPos[0] = 1;
    bool mode;

    bool patflag = 0;
    bool fileflag = 0;
    //bool que indica se o padrão virá de uma file
    bool patfile = 0;
    char *patpath;

    vector <string> mypats;

    string myFile;

    if ((strncmp(argv[1],"index",256) == 0)) {
        mode = 0;
        markedPos[1] = 1;
    }
    else if ((strncmp(argv[1],"search",256) == 0)) {
        mode = 1;
        markedPos[1] = 1;
    }
    else {
        cout << "Invalid Command" << "\n";
        showhelp();
        return 0;
    }

    for (int i = 2; i < argc; ++i) {
        if ((strncmp(argv[i],"-h",256) == 0) || (strncmp(argv[i],"--help",256) == 0)) {
            showhelp();
            return 0;
        }

        if ((strncmp(argv[i],"-p",256) == 0) || (strncmp(argv[i],"--pattern",256) == 0)) {
            // indica que já identificou de onde vem o padrão e que vem de uma file
            patflag = 1;
            patfile = 1;
            patpath = argv[i+1];
            // diz que essa posição dos args já foi identificada
            markedPos[i] = 1;
            markedPos[i+1] = 1;
        }
    }

    for (int j = 0; j < argc; j++) {
        if(markedPos[j] == 0 && (patflag == 0) && (mode==1)) {
            patflag = 1;
            markedPos[j] = 1;
            mypats.push_back(argv[j]);
        }
        if((markedPos[j] == 0) && (patflag == 1) && (mode==1)) {
            fileflag = 1;
            myFile = (argv[j]);
            markedPos[j]  = 1;
        }
        if((markedPos[j] == 0) && (fileflag == 0) && (mode==0)) {
            patflag = 1;
            fileflag = 1;
            myFile = (argv[j]);
            markedPos[j]  = 1;
        }

    }

    if ((patflag == 0) || (fileflag == 0)) {
        cout << "Invalid Command" << "\n";
        showhelp();
        return 0;
    }

    if(mode == 0) {
        string texto = "";
        string line;
        ifstream src(myFile);
        int nFile = myFile.length();
        string fileName = myFile.substr(0, nFile-4);
        string fileNameTxt = fileName + ".txt";
        string fileNameIdx = fileName + ".idx";

        int contLinha = 0;
        while(getline(src, line)) { //vai checar a linha para cada padrão
            if (contLinha == 0) {
                texto = texto + line;
            }
            else {
                texto = texto +"\n"+ line;
            }
            contLinha++;
        }

        char *txt = &texto[0u];
        int *sufArr = suffixArray(txt);
        int n = strlen(txt);
        cout << n << "\n";
        cout << txt << "\n";
        for (int i = 0; i < n; i++) {
            cout << sufArr[i] << "\n";
        }

        string encodedArr = compressIndexArray(sufArr, n);
        cout << encodedArr << "\n";

        // texto = char(0) + texto;
        // cout << texto.length() << "\n";
        string code = lz77(texto, 6, 4);
        cout << code << "\n";

        // string teste = encodeBits(15, 15);
        // cout <<  teste << "\n";
        // string my = "1010110";
        int m = code.length();
        int step = 0;

        ofstream outputFileTxt;

        outputFileTxt.open(fileNameTxt);

        while(m >= 8) {
            string myByte = code.substr((step*8), 8);
            bitset<8> b(myByte);
            // cout << b << " ";
            m -= 8;
            step ++;
            unsigned char bb = decodeBits(myByte);
            outputFileTxt.write((char*) &bb, 1);
            //write(outputFile, bb);
        }
        int m2 = code.length();
        int remain = m2 - (step*8);
        string myByte2 = code.substr(step*8, remain);
        cout << myByte2 << "\n";
        outputFileTxt.close();

        // char abc = '1';
        //
        // cout << (int)abc << "\n\n";

        // string my2 = char(64) + "";
        // bitset<8> c(my2.c_str()[0]);
        // bitset<60> b(code);
        // cout << b << endl;
        // cout << c << endl;

        // int num1 = 1000;
        // string num2 = "1000";
        // bool num3 = 1;
        // bool num3a = 0;
        // bitset<4> num4(num2);
        //
        // ofstream outputFile;
        // ofstream outputFile2;
        // ofstream outputFile3;
        // ofstream outputFile4;
        //
        // outputFile.open("resInt.idx");
        // outputFile << num1 << endl;
        // outputFile.close();
        //
        // outputFile2.open("resStr.idx");
        // outputFile2 << num2 << endl;
        // outputFile2.close();
        //
        // outputFile3.open("resBool.idx");
        // outputFile3 << num3;
        // outputFile3 << num3a;
        // outputFile3 << num3a;
        // outputFile3 << num3a << endl;
        // outputFile3.close();
        //
        // outputFile4.open("resBit.idx");
        // outputFile4 << num4 << endl;
        // outputFile4.close();
        ifstream src2(fileName);
        string testText = "";
        string line2;
        while(getline(src2, line2)) { //vai checar a linha para cada padrão
            testText = testText + line2;
        }
        cout << testText << "\n";
        int n5 = testText.length();
        for (int i = 0; i < n5; i++) {
            cout << (int)(unsigned char)(testText[i]) << "\n";
        }
        return 0;
    }

    return 0;
}
