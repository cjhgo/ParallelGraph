#include<string>
#include<vector>
#include<map>
#include <iostream>
#include <fstream>
#include<mutex>
#include <queue>

template <typename T>
class graph
{
public:
	graph(){};
	~graph(){};
	void show_graph();
	void load(std::string file_path, T data);
	bool directed = false;
	// void crate_mutex();
	int current_local_id = -1;
// private:
	
	std::map<int, int> global_id2local_id;
	int get_local_id(int global_id);

	
	std::vector <std::vector<T> > matrix;//(10, std::vector<bool>(10, false));
	std::vector <std::vector<bool> >matrix_flag;
	std::mutex matrix_mutex;
	std::queue<int> activate_vertexes;
	
	
	void add_edge(int source, int target, T data);
	void init_matrix_flag();
	int get_vertex_to_run();
	void resotore_vertex(int vertex);
};




template <typename T>
void graph<T>::load(std::string file_path, T data)
{
	std::ifstream infile(file_path);
	int source, target;
	while(infile >> source >> target)
	{

		if(this->directed)
		{
			this->add_edge(source,target,data);
		}
		else
		{
			this->add_edge(source, target,data);
			this->add_edge(target, source,data);
		}

	}
	this->init_matrix_flag();
}



template <typename T>
int graph<T>::get_local_id(int global_id)
{
	if(global_id2local_id.find(global_id) == global_id2local_id.end())
	{
		this->current_local_id++;
		global_id2local_id[global_id] = this->current_local_id;
		this->matrix.resize(this->current_local_id+1);
		for(auto& row : this->matrix)
			row.resize(this->current_local_id+1);
		return this->current_local_id;
	}
	else
	{
		auto search = global_id2local_id.find(global_id);
		return search->second;
	}

}

template <typename T>
void graph<T>::show_graph()
{
	for(auto x: this->global_id2local_id)
	{
		std::cout<<x.first<<"###"<<x.second<<std::endl;
	}
	std::cout<<this->matrix.size()<<std::endl;
	for(auto row : this->matrix)
	{
		for(auto ele : row)
		{
			std::cout<<ele<<"---";
		}
		std::cout<<std::endl;
	}
}

template <typename T>
void graph<T>::add_edge(int source, int target, T data)
{

	int source_local_id = get_local_id(source);
	int target_local_id = get_local_id(target);	
	this->matrix[source_local_id][target_local_id]=data;
}


// template <typename T>
// void graph<T>::crate_mutex()
// {
// 	this->matrix_mutex.resize(this->current_local_id+1);
// 	for(int i = 0; i <= this->current_local_id; i++)
// 	{
// 		this->matrix_mutex[i].resize(this->current_local_id+1);
// 		// for(int j = 0; j <= this->current_local_id; j++)
// 		// {
// 		// 	if(abs(this->matrix[i][j]-0) > 0.001)

// 		// }
// 	}

// }
template<typename T>
void graph<T>::init_matrix_flag()
{
	for (int i = 0; i <= this->current_local_id; ++i)
	{
		std::vector<bool> temp;
		for (int i = 0; i <= this->current_local_id; ++i)
		{
			temp.push_back(false);
		}
		this->matrix_flag.push_back(temp);
	}
	// this->matrix_flag = new std::vector <std::vector<bool> >(this->current_local_id, 
	// 								std::vector<bool>(this->current_local_id, false));

	for (int i = 0; i <= this->current_local_id; ++i)
	{
		this->activate_vertexes.push(i);
	}
}

template<typename T>
int graph<T>::get_vertex_to_run()
{
	this->matrix_mutex.lock();
	int count = 0;
	while(count <= this->current_local_id and this->activate_vertexes.size() > 0)
	{
		int vertex = this->activate_vertexes.front();
		this->activate_vertexes.pop();
		bool occupy = false;
		for(int i = 0 ; i <= this->current_local_id; i++)
		{
			auto& temp = this->matrix_flag;
			occupy = occupy || (temp[i][vertex]);
		}			
		for(int i = 0 ; i <= this->current_local_id; i++)
		{


			auto& temp = this->matrix_flag;
			occupy = occupy || (temp[vertex][i]);
		}
		if(not occupy)
		{

			for(int i = 0; i <= this->current_local_id; i++)
			{
				this->matrix_flag[vertex][i]=true;
			}
			for(int i = 0 ; i <= this->current_local_id; i++)
				this->matrix_flag[i][vertex] = true;
			this->matrix_mutex.unlock();
			return vertex;
		}
		else
			this->activate_vertexes.push(vertex);
				
		count++;
	}
	this->matrix_mutex.unlock();
	return -1;		

}
template<typename T>
void graph<T>::resotore_vertex(int vertex)
{
	this->matrix_mutex.lock();
	for(int i = 0; i <= this->current_local_id; i++)
		this->matrix_flag[vertex][i]=false;

	// for(auto& x : this->matrix_flag[vertex])
	// 	x = false;
	for(int i = 0 ; i <= this->current_local_id; i++)
		this->matrix_flag[i][vertex] = false;
	this->matrix_mutex.unlock();
}