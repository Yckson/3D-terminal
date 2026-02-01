#include <array>
#include <chrono>
#include <climits>
#include <cmath>
#include <cstddef>
#include <exception>
#include <fstream>
#include <istream>
#include <ncurses.h>
#include <bits/stdc++.h>
#include <sstream>
#include <stdexcept>
#include <string>
#include <thread>
#include <utility>
#include <vector>

constexpr double PI = 3.14159275358979323847;
WINDOW* win; 



using namespace std;

struct point_t{
    private: array<double, 3> cordinates;

    public: point_t() : cordinates{0, 0, 0}{};

    public: point_t (const pair<double, double>& cords) {
        this->cordinates = {cords.first, cords.second, 0};
    }

    public: point_t (const pair<int, int>& cords) {
        this->cordinates = {(double)cords.first, (double)cords.second, 0};
    }

    public: point_t (const array<double, 3>& cords) {
        this->cordinates = cords;
    }

    public: array<double, 3>& getCordinates () const {
        return const_cast<array<double, 3>&>(this->cordinates);
    }


    public: double& operator[](size_t axis){

        if (axis >= 0 && axis < 3) return this->cordinates[axis];
        else throw std::out_of_range("Eixo fora do intervalo [0-2]");
    }

    public: double& operator[](char axis){
        switch (axis) {
            case 'x': case 'X':
                return this->cordinates[0];
                break;
            case 'y': case 'Y':
                return this->cordinates[1];
                break;
            case 'z': case 'Z':
                return this->cordinates[2];
                break;
            default:
                throw std::out_of_range("Eixo fora do intervalor (x, y, z)");
                break;
        }
    }

};

class Screen {

    private: int width;
    private: int height;
    
    public: int FPS = 60;

    public: Screen (int width, int height) : height(height), width(width){

    }

    public: pair<int, int> cartesianToScreen (const pair<int, int>& cartesianCordinate) const {
        auto& [x, y] = cartesianCordinate;

        return {
            (x + this->width / 2),
            (y - this->height / 2) * -1
        };

    }

    public: pair<int, int> screenToCartesian (const pair<int, int>& screenCordinates) const{
        auto& [x, y] = screenCordinates;

        return {
            (x - this->width / 2),
            (y * -1) + this->height / 2
        };
    }

    public: pair<int, int> project (const point_t& cords) const {
        const double eps = 1e-9;
        const double camera_z = 5000;    // quanto mais alto, mais "longe" a câmera
        const double focal = 1000;       // escala para transformar x/z em pixels

        auto c = cords.getCordinates();
        double x = c[0];
        double y = c[1];
        double z = c[2];

        double zcam = z + camera_z;
        if (fabs(zcam) < eps) return {INT_MAX, INT_MAX};

        double px = (x / zcam) * focal;
        double py = (y / zcam) * focal;

        return { (int)std::round(px), (int)std::round(py) };
    }

    public: pair <int, int> getScreenSize (){
        return {this->width, this->height};
    }

    public: point_t translate (const point_t& p, double dz) const{
        auto [x, y, z] = p.getCordinates();
        return point_t ({x, y, z + dz});
    }

    public: point_t rotate (const point_t& p, double angle) const{
        auto [x, y, z] = p.getCordinates();
        const double c = cos(angle);
        const double s = sin(angle);
        double new_x = (x * c) - z * s;
        double new_z = x*s + z*c;
        return point_t ({
            (new_x * c) - y * s,
            new_x*s + y*c,
            new_z
        });
    }

    public: void drawLine (const point_t& from, const point_t& to) const {
        auto [x0_d, y0_d, z0] = from.getCordinates();
        auto [x_d, y_d, z] = to.getCordinates();

        int x0 = (int)std::round(x0_d);
        int y0 = (int)std::round(y0_d);
        int x = (int)std::round(x_d);
        int y = (int)std::round(y_d);

        int dx = abs(x - x0);
        int dy = abs(y - y0);

        int sx = (x0 < x) ? 1 : -1;
        int sy = (y0 < y) ? 1 : -1;
        int err = dx - dy;

        while (1){
            

            if (x0 == x && y0 == y) break;

            int e2 = 2 * err;

            if (e2 > -dy){
                err -= dy;
                x0 += sx;
            }

            if (e2 < dx){
                err += dx;
                y0 += sy;  
            }

            mvwprintw(win, y0, x0, "H");
        }
    }

};

struct ppmImage_t {

    int height = 0;
    int width = 0;
    vector<vector<int>> buffer;


    ppmImage_t (int height, int width, vector<vector<int>>& buffer) 
                : height(height), width(width), buffer(buffer){}

    ppmImage_t () : buffer(vector<vector<int>>()){}

    ppmImage_t (ppmImage_t& ppmImage) : height(ppmImage.height), width(ppmImage.width), buffer(ppmImage.buffer){}

    ppmImage_t (vector<vector<int>> buffer) : height(buffer.size()), width(buffer[0].size()), buffer(buffer){}

    ppmImage_t (int height, int width) : height(height), width(width){

        this->buffer = vector<vector<int>> (this->height, vector<int> (this->width, 1));

    }

    vector<int>& operator[] (size_t idx){
        if (idx < this->height){
            return this->buffer[idx];
        }
        else throw std::out_of_range("Img index out of range!\n");
    }
};



int main (int argc, char* argv[]){
    if (argc < 2) {
        cerr << "Uso: " << argv[0] << " <arquivo.obj>" << endl;
        return 1;
    }

    Screen scr = Screen(1880, 500);
    
    //cout << "p.first: " << p.first << " p.second: " << p.second << endl;

    initscr();
    cbreak();
    noecho();

    
    {
        auto [width, height] = scr.getScreenSize();
        win = newwin(height, width, 0, 0);
        box(win, 0, 0);
    }

    refresh();
    wrefresh(win);

    vector<point_t> points;
    vector<vector<int>> faces;

    fstream objFile (argv[1]);

    // Validar se arquivo foi aberto
    if (!objFile.is_open()) {
        endwin();
        cerr << "Erro: Arquivo '" << argv[1] << "' nao pode ser aberto!" << endl;
        cerr << "Verifique se o arquivo existe e o caminho esta correto." << endl;
        return 1;
    }

    string line;

    vector<vector<point_t>> objs;
    vector<point_t> currObj;
    istringstream tmps;
    string s;

    while (getline(objFile, line)){
        if (line.find("mtlib") != string::npos) continue;
        if (line[0] == '#') continue;
        else if (line[0] == 'o'){
            
        }
        else if (line[0] == 'v' && line.find("vn") == string::npos){
            tmps = istringstream (line); tmps >> s;
            double x, y, z; tmps >> x >> y >> z;
            points.push_back(point_t({x*100, y*80-10, z*90}));
        }
        else if (line[0] == 'f'){
            tmps = istringstream (line); tmps >> s;
            int vIndex = 0;
            istringstream tmps2;
            vector<int> face;

            for (int i = 0; i < 3; ++i){
                tmps >> s;
                tmps2 = istringstream (s);
                tmps2 >> vIndex;
                face.push_back(vIndex - 1);
            }

            faces.push_back(face);
        }

    }


    /*

    double h = 100, w = 200;

    

    vector <array<double, 3>> points = {
        {-w,-h ,w},
        {w, -h ,w},
        {w, h,w},
        {-w, h ,w},

        {-w, -h , -w},
        {-w, h ,-w},
        {w, h,-w},
        {w, -h,-w}
    };

    vector<vector<int>> faces = 
    {
        {0, 1, 2, 3},  // Frente (z=20)
        {7, 6, 5, 4},  // Trás (z=-20)
        {0, 1, 7, 4},  // Inferior (y=-10)
        {3, 2, 6, 5},  // Superior (y=10)
        {0, 4, 5, 3},  // Esquerda (x=-20)
        {1, 2, 6, 7}   // Direita (x=20)
    };

    */


    double dz = 0;
    double angle = 0;
    int i = 0;

    int counter = 1000;

    while (--counter){
        wrefresh(win);

        
        std::this_thread::sleep_for(chrono::milliseconds(1000/scr.FPS));

        auto p1 = scr.cartesianToScreen(scr.project(point_t ()));

        
         if (i == 3){
             wclear(win);
             i = 0;
         }
         i++;
        
        //wclear(win);
        //box(win, 0, 0);
        

        ///*

        ppmImage_t img(scr.getScreenSize().second, scr.getScreenSize().first);

        for (auto& face : faces){
            for (int i = 0; i < face.size(); i++){
                auto& pa = points[face[i]];
                auto& pb = points[face[(i+1)%face.size()]];


                auto paProjected = scr.cartesianToScreen(scr.project(scr.translate(scr.rotate(pa, angle), 1)));

                auto pbProjected = scr.cartesianToScreen(scr.project(scr.translate(scr.rotate(pb, angle), 1)));

                 scr.drawLine(
                     paProjected
                     ,
                     pbProjected
                 );
            }
        }

          for (auto& point : points){
              auto p = scr.cartesianToScreen(scr.project(scr.translate(scr.rotate(point, angle), 1)));
              if (p.second >= 0 && p.second < img.height && p.first >= 0 && p.first < img.width) {
                  mvwprintw(win, p.second, p.first, "@");
                  img[p.second][p.first] = 0;
              }
          }

        double dt =  1 / (double)scr.FPS;
        //dz += 1*dt;
        angle += (PI/4)*dt;

        ostringstream pathStream; pathStream << "./output/" << (1000 - counter) << ".ppm";
        string filePath = pathStream.str();
        ofstream tmpImg(filePath, ios::out);
        
        if (!tmpImg.is_open()) {
            mvwprintw(win, 0, 0, "Erro ao abrir arquivo: %s", filePath.c_str());
            wrefresh(win);
        } else {
            ostringstream imageStream;
            imageStream << "P1\n";
            imageStream << img.width << ' ' << img.height << '\n';

            for (int i = 0; i < img.height; i++){
                for (int j = 0; j < img.width; j++){
                    imageStream << img[i][j] << ' ';
                }
                imageStream << '\n';
            }

            tmpImg << imageStream.str();
            tmpImg.flush();
            tmpImg.close();
        }

        //sleep(10);
        
        //getch();


    }

    wclear(win);
    mvwprintw(win, 0, 0, "Framges gerados.");

    //getch();
    delwin(win);
    endwin();

    return 0;
}