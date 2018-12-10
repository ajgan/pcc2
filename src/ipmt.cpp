using namespace std;
#include <iostream>
#include <stdio.h>
#include <cstring>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <math.h>
#include <bitset>
#include <vector>

#define ALPHABET_LEN 256

// ---------- INDEX --------------------
struct suffix  {
    int index;
    string suf;
};

int compareSuffix(struct suffix a, struct suffix b) {
    return ((a.suf).compare(b.suf)) < 0? 1 : 0;
}

vector <int> suffixArray(string txt) {
    int n = txt.length();
    vector <suffix> suffixes;

    for (int i = 0; i < n; i++) {
        suffix mypoint = {i, txt.substr(i, 16)};
        suffixes.push_back(mypoint);
    }

    sort(suffixes.begin(), suffixes.end(), compareSuffix);

    vector <int> suffixArr;
    for (int i = 0; i < n; i++) {
        suffixArr.push_back(suffixes[i].index);
    }

    return  suffixArr;
}

// ---------- COMPRESS --------------------

string encodeBits(int size, int n) {
    bool power2 = ((size & (size - 1)) == 0);
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
    return test;
}

string lz77(string txt, int window, int buff) {
    //cout << "lz77";
    string w;
    for (int a = 0; a < window; a++) {
        w += char(0);
    }
    txt = w + txt;
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
        string result2 = encodeBits(window, pos) + encodeBits(buff, len) + encodeBits(255, int(letter));
        code2 += result2;
    }
    return code2;
}

string compressIndexArray (vector <int> sufArr, int len) {
    string result = "";
    for (int i = 0; i < len; i++) {
        result += encodeBits(len - 1, sufArr[i]);
    }
    return result;
}

// ---------- DECOMPRESS --------------------
int bin_to_dec(vector <int> bin){
    int l = bin.size() - 1;
    int num = 0;
    for(int i = 0; i <= l; i++){
        if(bin[i] == 1){
            num += pow(2, (l-i));
        }
    }

    return num;
}


vector <int> dec_to_bin(int num){
    vector <int> bin;

    for(int i = 7; i >= 0; --i){
        int k = num >> i;
        //int a = (k & 1);
        //cout << (k & 1);
        bin.push_back((k & 1));
        //cout << (k & 1);
    }
    //cout << endl;
    return bin;
}

vector <int> get_tripla(unsigned char c1, unsigned char c2, unsigned char c3){
    vector <int> cb1 = dec_to_bin(c1);
    vector <int> cb2 = dec_to_bin(c2);
    vector <int> cb3 = dec_to_bin(c3);
    vector <int> out;

    for (int i = 0; i < cb2.size(); i++){
        cb1.push_back(cb2[i]);
    }

    for (int i = 0; i < cb3.size(); i++){
        cb1.push_back(cb3[i]);
    }

    //cout << "Size Total " << cb1.size() << endl;

    //cout << "Binario concact" << endl;

    //for(int i = 0; i < cb1.size(); i++){
    //    cout << cb1[i];
    //}
    //cout << endl;

    vector <int> wb;
    vector <int> wr;
    vector <int> ch;

    for(int i = 0; i < 9; i++){
        wb.push_back(cb1[i]);
    }

    for(int i = 9; i < 16; i++){
        wr.push_back(cb1[i]);
    }

    for(int i = 16; i < cb1.size(); i++){
        ch.push_back(cb1[i]);
    }
    //cout << "size " << wb.size() << endl;
    //cout << "size " << wr.size() << endl;
    //cout << "size " << ch.size() << endl;

    int blue = bin_to_dec(wb);
    int red = bin_to_dec(wr);
    int caract = bin_to_dec(ch);

    out.push_back(blue);
    out.push_back(red);
    out.push_back(caract);

    return out;
}

vector <vector <int> > all_triplas(string code){
    int lim = code.size();
    vector <vector <int> > lz;

    for(int i = 0; i < lim; i+=3){
        vector <int> tri = get_tripla(code[i], code[i+1], code[i+2]);

        // for(int j = 0; j<tri.size(); j++){
        //     cout << "tripla " << j << " " << tri[j] << endl;
        // }

        lz.push_back(tri);
    }

    return lz;
}

string lz77_decode(int ls, int ll, string fileName){
    string txt = "";
    int l1 = ALPHABET_LEN;
    int sb_init = 0;
    FILE *file;
    FILE *out;
    out = fopen((fileName + "_decoded.txt").c_str(), "wt");
    file = fopen((fileName + "lz.txt").c_str(), "rt");
    unsigned char c1, c2, c3;
    int v = 0;

    //ifstream t("textlz.txt");
    //ostringstream buffer;
    //buffer << t.rdbuf();
    //string etxt = "";
    //etxt = buffer.str();
    //cout << etxt.length() << endl;

    while(fscanf(file, "%c%c%c", &c1, &c2, &c3) == 3){
        // cout << c3 << endl;
        v+=1;
        vector <int> tripla = get_tripla(c1, c2, c3);
        int p = tripla[0];
        int l = tripla[1];
        unsigned char c = tripla[2];

        for(int j = 0; j < l; j++){
            //cout << (sb_init-p+j) << endl;
            txt += txt[sb_init-p+j];
            fprintf(out, "%c", txt[sb_init-p+j]);
        }

        txt += c;
        fprintf(out, "%c", c);
        sb_init += (l+1);
    }

    //cout << "TEST " << v;
    /*for(int i = 0; i< triplas.size(); i++){
        int p = triplas[i][0];
        int l = triplas[i][1];
        unsigned char c = triplas[i][2];

        for(int j = 0; j < l; j++){
            //cout << (sb_init-p+j) << endl;
            txt.push_back(txt[sb_init-p+j]);
        }
        txt.push_back(c);
        sb_init += (l+1);
    }*/

    //vector <char> out(txt.begin() + ls, txt.end());

    return txt;
}

// --------- INDEX DECODER/SEARCHER ------

vector <int> dec_to_bin(int num, int tam){
    vector <int> bin;

    for(int i = tam-1; i >= 0; --i){
        int k = num >> i;
        //int a = (k & 1);
        //cout << (k & 1);
        bin.push_back((k & 1));
        //cout << (k & 1);
    }
    //cout << endl;
    return bin;
}

void print_vector(vector <int> v){
    for(int i = 0; i < v.size(); i++){
        cout << v[i];
    }
    cout << endl;
}

char *get_text(string name){
    ifstream t(name);
    ostringstream buffer;
    buffer << t.rdbuf();
    string etxt = "";
    etxt = buffer.str();
    // cout <<"Teste n caracteres " << etxt.length() << endl;
    int textlen = etxt.length();
    char *text = new char[textlen];
    for(int i = 0; i<textlen; i++){
        text[i] = etxt[i];
    }

    return text;
}

bool *get_bin(string name, int tam, int binWindow){
    FILE *idx;
    char name2[name.size()];
    for(int i = 0; i<name.size(); i++){
        name2[i] = name[i];
    }
    idx = fopen(name2, "rt");
    char c;
    bool *bin = new bool[tam*binWindow];
    int s = 0;
    while(fscanf(idx, "%c", &c) == 1){
        vector <int> x = dec_to_bin(c, binWindow);
        for(int i = 0; i<binWindow; i++){
            bin[s+i] = x[i];
        }
        s += binWindow;
    }
    // cout << "TESTE BIN " << bin[binWindow*tam] << endl;
    return bin;
}

int *suffix_array(bool *bin, int binWindow, int tam){
    vector <int> suf;
    for(int i = 0; i < tam*binWindow; i += binWindow){
        vector <int> x;
        for(int j = i; j<i+binWindow; j++){
            x.push_back(bin[j]);
        }

        int num = bin_to_dec(x);
        suf.push_back(num);
    }

    int *sarray = new int[suf.size()];

    for (int i = 0; i<suf.size(); i++){
        sarray[i] = suf[i];
    }

    return sarray;
}

int compare(char *a, char *b, int n){
    for (int i = 0; i<n; i++){
        if(a[i] > b[i]) return 1;
        else if (a[i] < b[i]) return -1;
    }
    return 0;
}

int search(char *pat, char *txt, int *sarray, int n){
    int m = strlen(pat), ll = 0, lr = n-1;

    while(ll <= lr){
        int med = ll + (lr-ll)/2;
        int out = compare(pat, txt+sarray[med], m);

        if(out == 0) return sarray[med];

        else if(out > 0) ll = med + 1;
        else if(out < 0) lr = med - 1;
    }

    return -1;

}

// --------GENERAL --------------

void showhelp() {
    string my_val = "Formato de comando:\n"
    "./ipmt index [options] textfile || ./ipmt search [options] pattern indexfile\n"
    "Flags de opção do modo de busca(search):\n"
    "-c ou --count\n"
    "    Exibe contagem de ocorrências do padrão no texto(por default, não se exibe)\n"
    "-p ou --pattern patternfile\n"
    "     Indica que os padrões devem vir de um arquivo patternfile\n"
    "-h ou --help\n"
    "     Exibe informações básicas da ferramenta\n"
    "-t ou --text\n"
    "     Comprime o texto, sem indexar os sufixos\n"
    "-s ou --suffix\n"
    "     Indexa os sufixos, mas não comprime o texto\n";
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

    bool patfile = 0;
    char *patpath;
    bool printCont = 0;

    bool doSuffix = 1;
    bool doCompress = 1;

    vector <char*> mypats;

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

        else if ((strncmp(argv[i],"-p",256) == 0) || (strncmp(argv[i],"--pattern",256) == 0)) {
            patflag = 1;
            patfile = 1;
            patpath = argv[i+1];
            markedPos[i] = 1;
            markedPos[i+1] = 1;
        }
        else if ((strncmp(argv[i],"-c",256) == 0) || (strncmp(argv[i],"--count",256) == 0)) {
            printCont = 1;
            markedPos[i] = 1;
        }
        else if ((strncmp(argv[i],"-s",256) == 0) || (strncmp(argv[i],"--suffix",256) == 0)) {
            doCompress = 0;
            markedPos[i] = 1;
        }
        else if ((strncmp(argv[i],"-t",256) == 0) || (strncmp(argv[i],"--text",256) == 0)) {
            doSuffix = 0;
            markedPos[i] = 1;
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

    int nFile = myFile.length();
    string fileName = myFile.substr(0, nFile-4);
    string fileNameTxt = fileName + "lz.txt";
    string fileNameIdx = fileName + ".idx";

    //INDEX MODE
    if(mode == 0) {

        string texto = "";
        string line;

        int contLinha = 0;


        ifstream t(myFile);
        ostringstream buffer;
        buffer << t.rdbuf();

        texto = buffer.str();

        t.close();
        t.clear();

        if (doSuffix) {
            vector <int> sufArr = suffixArray(texto);
            int n = texto.length();

            int indStep = 0;
            string encodedArr = compressIndexArray(sufArr, n);

            ofstream outputFileIdx;
            outputFileIdx.open(fileNameIdx, ofstream::trunc);
            int encLen = encodedArr.length();
            string myIndByte;
            while(encLen >= 8) {
                myIndByte = encodedArr.substr((indStep*8), 8);

                encLen -= 8;
                indStep ++;
                unsigned char ibb = decodeBits(myIndByte);
                outputFileIdx.write((char*) &ibb, 1);
            }
            int encLen2 = encodedArr.length();
            int indRemain = encLen2 - (indStep*8);
            string myIndByte2 = encodedArr.substr(indStep*8, indRemain);
            int remainCont = 0;
            for (int i=0; i<(8-indRemain); i++) {
                myIndByte2 = myIndByte2 + "0";
                remainCont++;
            }
            indStep ++;
            unsigned char ibbRemain = decodeBits(myIndByte2);
            outputFileIdx.write((char*) &ibbRemain, 1);

            int encLen3 = encodedArr.length() + remainCont;
            int indRemain2 = encLen3 - (indStep*8);
            string myIndByte3 = encodedArr.substr((indStep-1)*8, indRemain2);
            cout << myIndByte3;
            outputFileIdx.close();
        }

        if (doCompress) {
            string code = lz77(texto, 511, 127);
            int m = code.length();
            int step = 0;

            ofstream outputFileTxt;

            outputFileTxt.open(fileNameTxt, ofstream::trunc);
            string myByte;
            while(m >= 8) {
                myByte = code.substr((step*8), 8);
                m -= 8;
                step ++;
                unsigned char bb = decodeBits(myByte);
                outputFileTxt.write((char*) &bb, 1);

            }
            int m2 = code.length();
            int remain = m2 - (step*8);
            string myByte2 = code.substr(step*8, remain);
            cout << myByte2;
            outputFileTxt.close();
        }

        // string texto2 = "";
        // ifstream t2("textlz.txt");
        // ostringstream buffer2;
        // buffer2 << t2.rdbuf();
        // //FILE READ
        //
        // texto2 = buffer2.str();
        // cout << texto2.length() << "\n";
        //
        // string sub2 = texto2.substr(0, 5);
        // for (int i=0; i<sub2.length(); i++) {
        //     cout << (int)(unsigned char)sub2[i] << "\n";
        // }

        return 0;
    }
    //SEARCH MODE
    else {
        // string code = "";
        char *fileName2 = &fileName[0u];


        string text = lz77_decode(512, 128, fileName);

        // string a = "abd";
        // string b = "abe";
        // char txt[] = "baca";
        // char *txt2 = new char[6];
        // for(int i= 0; i< strlen(txt); i++){
        //     txt2[i] = txt[i];
        // }
        // char pat[] = "ana";
        // vector <int> x = {5,3,1,0,4,2};
        // int *sarray = new int[strlen(txt)];
        //
        // for (int i = 0; i<x.size(); i++){
        //     sarray[i] = x[i];
        // }
        //
        // cout << search(pat, txt2, sarray, strlen(txt)) << endl;
    //***********************************************************************************************************************

        char *textC = get_text(fileName+".txt");
        int tam = text.length();
        bool power2 = ((tam & (tam - 1)) == 0);
        int binWindow = ceil(log2(tam));
        if(power2) binWindow++;
        bool *bin = get_bin(fileName+".idx", tam, binWindow);
        int *sarray2 = suffix_array(bin, binWindow, tam);

        for (int j = 0; j < mypats.size(); j++) {
            int out = search(mypats[j], textC, sarray2, tam);
            cout << "Ocorrências: " << out << endl;
        }

        int s = 0;
        string code;

        // cout <<"Numero de bits: " << binWindow*tam << endl;
        //vector <int> test = dec_to_bin(strlen(text), binWindow);

        //print_vector(test);

        // for(int i=0; i < 300; i++){
        //     cout << text[i];
        // }
        // cout << endl;

        return 0;
    }

    return 0;
}
