// Below is the syntax to use the 
// nested namespace in one line 

/*
namespace Game { 
  
    namespace Graphics { 
  
        namespace Physics { 
  
           class 2D { 
           }; 
        } 
    } 
}
*/

  
namespace Game::Graphics::Physics { 
  
    class 2D { 

    }; 
} 

int main(){}
