using namespace std;
#include <iostream>
#include <stdio.h>
#include <cstring>
#include <string>
#include <fstream>

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

// bool findPref(pref, txt, j, n, window, windPos) {
//     compTxt = "";
//     for (int i = windPos; i < windPos+window; i++) {
//         if(i >= 0) {
//
//         }
//     }
//     if (strncmp(pref,"-h",256) == 0) {
//         return 1
//     }
//     return 0;
// }
string lz77(string txt, int window, int buff) {

    string w;
    for (int a = 0; a < window; a++) {
        w += char(0);
    }
    txt = w + txt;
    string code = "";
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
        code += result;
        // if(len > 0) i += len - 1;
    }
    return code;
}
int main() {
    string texto = "";
    string line;
    ifstream src("text.txt");
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
    int *a = suffixArray(txt);
    int n = strlen(txt);
    cout << n << "\n";
    cout << txt << "\n";
    for (int i = 0; i < n; i++) {
        cout << a[i] << "\n";
    }

    // texto = char(0) + texto;
    // cout << texto.length() << "\n";
    string code = lz77(texto, 6, 4);
    cout << code << "\n";
    return 0;
}
