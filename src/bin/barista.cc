#include "base/kaldi-type-info.h"
#include "io/vector-writer.h"
#include "io/matrix-writer.h"
#include "io/command-line-interface.h"
#include "io/file-list-reader.h"
#include "io/portaudio-reader.h"
#include "io/pcm-reader.h"
#include "feat/compute-mfcc-feats.h"
#include "feat/apply-cmvn.h"
#include "feat/add-deltas.h"
#include "feat/filter.h"
#include "gmm/gmm-decode-faster-online.h"
#include "util/common-utils.h"
#include "graphviz/gvc.h"
#include "boost/property_tree/ini_parser.hpp"

using namespace std;
using namespace cppa;
using namespace kaldi;

using boost::property_tree::ptree;
using boost::property_tree::ini_parser::read_ini;

typedef vector<string> edge_list_type;
typedef pair<string, edge_list_type> vertex_type;
typedef vector<vertex_type> adjacency_list_type;

// Reads a graph in DOT (Graphviz) format.
bool ReadGraph(string filename, adjacency_list_type* graph) {
  FILE* fp;
  fp = fopen(filename.c_str(), "r");
  if (fp == NULL) {
    cerr << "Could not open graph file: " << filename << endl;
    return false;
  }
  // Read the graph and construct an adjacency list
  Agraph_t* g = agread(fp, 0);
  fclose(fp);
  for (Agnode_t* n = agfstnode(g); n; n = agnxtnode(g, n)) {
    edge_list_type edges;
    for (Agedge_t* e = agfstout(g, n); e; e = agnxtout(g, e)) {
      edges.push_back(agnameof(aghead(e)));
    }
    graph->push_back(make_pair(agnameof(n), edges));
  }
  agclose(g);
  return true;
}

// Spawns actors
bool SpawnActors(adjacency_list_type& graph, ptree& config,
                 map<string, actor_ptr>* actor_map,
                 map<string, group_ptr>* subscribers_map) {
  
  // Spawn an actor for each node in the graph
  for (int u = 0; u < graph.size(); u++) {
    string actor_name = graph[u].first;
  	string actor_type = config.get<string>(actor_name + ".actor_type");
    // aout << actor_name << " : " << actor_type << endl;
    
    // Create a subscribers group for the actor to be spawned
    auto subscribers = group::anonymous();
    subscribers_map->insert(make_pair(actor_name, subscribers));
    
    // Spawn the actor
    if (actor_type == "FileListReader") {
      actor_map->insert(make_pair(actor_name, spawn<FileListReader>(actor_name, subscribers, config)));
    } else if (actor_type == "CommandLineInterface") {
      actor_map->insert(make_pair(actor_name, spawn<CommandLineInterface>(actor_name, subscribers, config)));
    } else if (actor_type == "PaReader") {
      actor_map->insert(make_pair(actor_name, spawn<PaReader>(actor_name, subscribers, config)));
    } else if (actor_type == "PCMReader") {
      actor_map->insert(make_pair(actor_name, spawn<PCMReader>(actor_name, subscribers, config)));
    } else if (actor_type == "ComputeMFCCFeats") {
      actor_map->insert(make_pair(actor_name, spawn<ComputeMFCCFeats>(actor_name, subscribers, config)));
    } else if (actor_type == "ApplyCMVN") {
      actor_map->insert(make_pair(actor_name, spawn<ApplyCMVN>(actor_name, subscribers, config)));
    } else if (actor_type == "AddDeltas") {
      actor_map->insert(make_pair(actor_name, spawn<AddDeltas>(actor_name, subscribers, config)));
    } else if (actor_type == "Filter") {
      actor_map->insert(make_pair(actor_name, spawn<Filter>(actor_name, subscribers, config)));
    } else if (actor_type == "GMMDecodeFasterOnline") {
      actor_map->insert(make_pair(actor_name, spawn<GMMDecodeFasterOnline>(actor_name, subscribers, config)));
    } else if (actor_type == "VectorWriter") {
      actor_map->insert(make_pair(actor_name, spawn<VectorWriter>(actor_name, subscribers, config)));
    } else if (actor_type == "MatrixWriter") {
      actor_map->insert(make_pair(actor_name, spawn<MatrixWriter>(actor_name, subscribers, config)));
    } else {
      aout << "Unknown actor type: " << actor_type << endl;
      return false;
    }    
  }
  return true;
}

// Run actors after setting up group subscriptions
bool RunActors(adjacency_list_type& graph,
               map<string, actor_ptr>* actor_map,
               map<string, group_ptr>* subscribers_map,
               group_ptr actors) {
  
  // Run each actor in the graph
  for (int u = 0; u < graph.size(); u++) {
    string actor_name = graph[u].first;
    
    // Add the actor to the actors group
    sync_send((*actor_map)[actor_name], atom("JOIN"), actors).await(
      on(atom("JOINED")) >> [&]() { /* handle */ },
      after(chrono::seconds(30)) >> [&] {
        aout << "Actor" << actor_name << 
                " failed to subscribe to the actors group." << endl;
        return false;
      }
    );    
    
    // Iterate over the subscribers of the current actor
    for (int v = 0; v < graph[u].second.size(); v++) {
      string subscriber_name = graph[u].second[v];
      // Ask each subscriber to join the subscribers group
      sync_send((*actor_map)[subscriber_name], 
                atom("JOIN"), (*subscribers_map)[actor_name]).await(
        on(atom("JOINED")) >> [&]() { /* handle */ },
        after(chrono::seconds(30)) >> [&] {
          aout << "Actor " << subscriber_name << 
                  " failed to subscribe to actor" <<  actor_name << endl;
          return false;
        }
      );
      // Create a link between the actor and the subscriber
      sync_send((*actor_map)[subscriber_name], 
                atom("LINK"), (*actor_map)[actor_name]).await(
        on(atom("LINKED")) >> [&]() { /* handle */ },
        after(chrono::seconds(3)) >> [&] {
          aout << "Actor " << subscriber_name << 
                  " failed to link to actor" <<  actor_name << endl;
          return false;
        }
      );
    }
    
    // Send the RUN message to the current actor now that 
    // all of its subscribers are added to its subscribers group.
    send((*actor_map)[actor_name], atom("RUN"));
  }
  return true; 
}

int main(int argc, char *argv[]) {
  try {
    const char *usage =
      "Launch a barista instance.\n"
      "Usage:  barista [options] module-config.ini [module-graph.dot]\n"
      "Note: module-config is a configuration file in INI format.\n"
      "      module-graph  is a directed graph in DOT (GraphViz) format.";
    ParseOptions po(usage);
    po.Read(argc, argv);
  
    if (po.NumArgs() != 2) {
      po.PrintUsage();
      exit(1);
    }

    string config_filename = po.GetArg(1), graph_filename = po.GetArg(2);
  
    // Read actor config
  	ptree config;
  	read_ini(config_filename, config);
  
    // Read actor graph
    adjacency_list_type graph;
    if(!ReadGraph(graph_filename, &graph))
      exit(1);
  
    // Announce kaldi types to the libcppa type system;
    AnnounceKaldiTypes();
    
    // Spawn and run actors
    map<string, actor_ptr> actor_map;
    map<string, group_ptr> subscribers_map;
    auto actors = group::anonymous();
    if (!SpawnActors(graph, config, &actor_map, &subscribers_map) ||
        !RunActors(graph, &actor_map, &subscribers_map, actors)) {
      exit(1);
    }
    
    // Send the START message to the actors group
    send(actors, atom("START"));
    
    // Wait until other actors are done
    await_all_others_done();
    
    // Clean up before exiting main
    shutdown();
    
    aout << " ++  Done" << endl;
    return 0;
  } catch (const exception &e) {
    cerr << e.what();
    return -1;
  }
}
