#include <iostream>
#include <cmath>
#include <fstream>
#include <string>
using namespace std;

//#define DEBUG

//tengo conto anche delle rotazioni per sapere il numero di passi!	
#define MAX_FIT_1 300 //distanza
#define MAX_FIT_2 0.25   // 1/n° di passi--così ho un minimo di 4 passi
#define X_GOAL_ 106
#define Y_GOAL_ 54

//muro
// 80- -10
// 80-90
// 82- -10
// 82-90


bool test_intersezione(float x11, float y11,float x12, float y12,float x21, float y21,float x22, float y22,float *X,float *Y);

int main (int argc, char** argv) {

	float x,y,x_new,y_new,x_goal=X_GOAL_, y_goal=Y_GOAL_; //posizione iniziale sul piano cartesiano
	float X_intersez,Y_intersez;
  
  	ofstream percorso; //contiene il percorso seguito da un dato individuo
  	ofstream ofile("fitness.output");
  	ofstream myfitness("MYfitness.output",ios::app); //elenco di tutte le fitness generate, mi serve per generare grafici
  
  	string s;
  	
  	float direzione,ruota_di;
  	float spostamento;
  	bool limiti_superati,muro_attraversato,simula=false;
  
  	float COMP1,COMP2;// perchè è 1/numero_passi
  
  	long int numero_passi;
  
  	int i;
  	float fitness; 
    
  	s.erase();
  	s.append(argv[1]);
  
  	if(s == "simula") {
  		i = 2;
    	simula = true;
    	cout << "\n Calcolo il percorso che effettua l'individuo "<<argv[2]<<endl;
    
    	s.erase();
//      s.append("coordinate_");
//      s.append(argv[2]).append(".csv");

     	s.append("coordinate.txt");
    
	    percorso.open(s.c_str(),fstream::trunc);
    	percorso << "0.0 0.0"<< endl;
  	}
  	else
    	i = 1;
  
  	for(; i < argc; i++) { //per ogni file 
		//azzero variabili
  		x=0;
  		y=0;
  		direzione=0;
  		spostamento=0;
  		numero_passi=0;
  		limiti_superati=false;
  		muro_attraversato=false;
  
  		ifstream file(argv[i]);
  
  		while(file.eof() == false) {
    		s = '\0'; 
    		file >> s;
    
    
    		if(s == "move") {
      
      			file >> spostamento;
      
      			if(spostamento != 0)
					numero_passi++;
      
#ifdef DEBUG
	      		cout <<endl<< "Posizione: (" << x <<"," << y <<")"  << endl;
#endif
      
	      		//calcolo nuova posizione
      			x_new= x + spostamento*cos(direzione);
      			y_new= y + spostamento*sin(direzione); 
      
#ifdef DEBUG
      			cout <<endl<< "Posizione partenza: (" << x <<"," << y <<")"  << endl;
      			cout <<endl<< "Posizione arrivo: (" << x_new <<"," << y_new <<")"  << endl;
#endif
      
      			//prima i due punti della retta spostamento poi i quattro casi possibili delle rette del quadrato
      			if(test_intersezione(x,y,x_new,y_new,80,-10,80,90,&X_intersez,&Y_intersez)) {
	
#ifdef DEBUG
					cout << "\nAttenzione: attraversato muro , perché lo interseco in:  "<< X_intersez << " _ "<< Y_intersez << endl;
#endif
					muro_attraversato=true;	
	
      			}
      
    	  		if(test_intersezione(x,y,x_new,y_new,80,90,82,90,&X_intersez,&Y_intersez)) {
	
#ifdef DEBUG
					cout << "\nAttenzione: attraversato muro , perché lo interseco in:  "<< X_intersez << " _ "<< Y_intersez << endl;
#endif
					muro_attraversato=true;		
      			}	
        
 		    	if(test_intersezione(x,y,x_new,y_new,82,90,82,-10,&X_intersez,&Y_intersez)) {
	
#ifdef DEBUG
					cout << "\nAttenzione: attraversato muro , perché lo interseco in:  "<< X_intersez << " _ "<< Y_intersez << endl;
#endif
					muro_attraversato=true;	
		    	}
      
		    	if (test_intersezione(x,y,x_new,y_new,82,-10,80,-10,&X_intersez,&Y_intersez)) {
	
#ifdef DEBUG
					cout << "\nAttenzione: attraversato muro , perché lo interseco in:  "<< X_intersez << " _ "<< Y_intersez << endl;
#endif
					muro_attraversato=true;
      			}
      
 		    	if(x_new<0 ||x_new>120 ||y_new<0 ||y_new>100) {
#ifdef DEBUG
					cout << "\nAttenzione: superati limiti spazio di ricerca! - "<< x << " _ "<< y << endl;
#endif
					muro_attraversato=true;
		    	}
      
	        	if(!muro_attraversato) {
	
					x=x_new;
					y=y_new;
		    	}
         
      			if(simula && percorso!=nullptr)
					percorso << x << " "<< y << endl;
    		}
    		else if(s == "rotate") {
      
				file >> ruota_di;
      
		    	if(ruota_di!=0)
					numero_passi++;
      
				direzione+=ruota_di;   
      
#ifdef DEBUG
		    	cout<<"ruota di : "<< ruota_di<< endl;
      			cout <<endl<< "Rotazione: " << direzione << endl;
#endif      
      	
    		}  	
  		}
    
     
    COMP1 = 300.0f - sqrt(pow(x-x_goal,2)+pow(y-y_goal,2));
    
    if(300.f-COMP1<=0.5) //metto una soglia di precisione
      COMP1=300.f;
    
    if(numero_passi!=0)
    	COMP2=1.0f/numero_passi; //così dovrebbe evitare il muro?
	else COMP2=0;
    
    if(simula)  
      cout<< COMP1 <<" " << COMP2 ;     
    
    //stesso peso
    
    COMP1=COMP1/300.0f *0.5;
    
    COMP2= COMP2 *0.5;
   
    //     ofile <<fitness<<" "<<COMP2<<" "<<COMP1  <<endl ;   //metto le componenti come promemoria
    //     myfitness   <<fitness<<endl;
    
    //due componenti
    COMP1=max(0.1f,COMP1);
    COMP2=max(0.000000001f,COMP2);
    ofile << COMP1 << " " << COMP2 << endl;
    myfitness << COMP1 << " " << COMP2 << endl;
    
    if(simula)  
      cout<<" norm: " <<COMP1 <<" " << COMP2<< " fitness "<< fitness <<endl ;     
   
   
  	file.close();
  	}
  
  	ofile.close();
  	myfitness.close();
  
  	if(percorso.is_open())
  		percorso.close();
  
  	return 0;
}

bool test_intersezione(float Ax, float Ay,float Bx, float By,float Cx, float Cy,float Dx, float Dy,float *X,float *Y){
  //due rette  
  //AB
  //CD
  
  float m1,m2,q1,q2;
  
  m1= (By-Ay) / (Bx-Ax);
  m2= (Dy-Cy) / (Dx-Cx);
  
  q1= Ay - Ax * (By-Ay)/(Bx - Ax);
  q2= Cy - Cx * (Dy-Cy)/(Dx - Cx);
  
  if(Ax==Bx) { //prima retta verticale
    
    *X=Ax;
    
    *Y=m2 * *X+q2 ; //calcolo l'intersezione con l'altra retta...
  }

  else
    if(Cx==Dx) { //seconda retta verticale
      
      *X=Cx;
      
      *Y=m1 * *X+q1 ; //calcolo l'intersezione con l'altra retta...  
    }

    else { //caso normale
	
	    if((m1-m2)==0.0)
			return false; //sono parallele
	
		*X=(q2-q1)/(m1-m2);
    	*Y=m1**X+q1;
    }
    
    //siccome sono segmenti devo controllare la posizione delle intersezioni, perchè potrebbero essere all'infinito
    if(min(Ax,Bx) <= *X &&  *X <= max(Ax,Bx) &&  min(Ay,By) <= *Y && *Y <= max(Ay,By) && min(Cx,Dx) <= *X &&  *X <= max(Cx,Dx) &&  min(Cy,Dy) <= *Y && *Y <= max(Cy,Dy) )
     	return true;
    else 
		return false;	//non si intersecano
     
}














