#include "graph.h"
#include "parkour.h"

// function to test the Graph structure
void test_Graph (const std::string & s)
{
  // load the instance
  Graph g(s);

  // display it
  std::cout << g << std::endl;
}


// function to remind the things to be done / finished
void reminder ()
{
  std::cout << "done: basic graph functionalities (read,display)" << std::endl;
  std::cout << "fix: check remaining bugs" << std::endl;
  std::cout << "to do: implement Dijkstra" << std::endl;
  std::cout << "to do: implement Bellman-Ford" << std::endl;
  std::cout << "to do: adapt the structure to solve the scheduling problem" << std::endl;
}


// main function
int main (int argc, char * argv[])
{
	std::string filename("example.dat");

	float distance[100];
	Arc* pere[100];

	// check the command line
	if (argc > 2)
	{
		std::cerr << "Error: expecting one single argument (instance name)" << std::endl;
		return 1;
	}
	else if (argc == 2)
	{
		filename = argv[1];
	}

	// test
	test_Graph(filename);

	//reminder ();

	return 0;
}
