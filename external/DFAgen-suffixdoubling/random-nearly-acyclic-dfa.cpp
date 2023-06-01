#include <iostream>
#include <unistd.h>
#include <vector>
#include <cstdlib>
#include <algorithm>
#include <chrono>

using namespace std;

int rand_range( int n ) {
    return rand() % n;
}

void generate( int n, int sigma, int indeg, int add_random_edge, bool acyclic, bool shuffle, bool debug ) {

    vector<int> ID(n);
    for( int v = 0; v < n; ++v ) { ID[v] = v; }
    if( shuffle ) {
        random_shuffle( ID.begin(), ID.end() );
    }

    vector< int > available_u;
    vector< int > outdegree( n, 0 );
    vector< bool > final_states( n, true );
    vector< pair<int,int> > edges;

    available_u.reserve( n );
    edges.reserve( n*indeg+add_random_edge );

    int remaining_edges = n*sigma;

    for( int v = 0; v < n; ++v ) {
        int d = min(indeg,v);
        while( !available_u.empty() && (d--) ) {
            int U = available_u.size();
            int i = rand_range(U);
            int u = available_u[i];

            edges.push_back( make_pair( ID[u], ID[v] ) );
            final_states[ID[u]] = false;

            if( ++outdegree[ID[u]] == sigma ) {
                available_u[i] = available_u[ U-1 ];
                available_u.resize( U-1 );
            }
        }

        available_u.push_back( v );
    }

    while( !available_u.empty() && add_random_edge ) {
        int U = available_u.size();
        int i = rand_range(U);
        int u = available_u[i];

        int v = u;
        if( acyclic ) {
            if( U == 1 ) break;
            if( v == n-1 ) continue;
            v = rand_range(n-v-1)+v+1;
        } else {
            v = rand_range(n);
        }

        edges.push_back( make_pair( ID[u], ID[v] ) );
        //final_states[ID[u]] = false;

        if( ++outdegree[ID[u]] == sigma ) {
            available_u[i] = available_u[ U-1 ];
            available_u.resize( U-1 );
        }

        --add_random_edge;
    }

    vector<int> F;
    for( int v = 0; v < n; ++v ) {
        if( final_states[v] ) F.push_back(v);
    }

    cout << n + 1 << ' ' << edges.size() + 1 << ' ' << ID[0] << ' ' << F.size() << endl;
    // ## add dummy source
    cout << ID[0] << ' ' << 0 << ' ' << ID[0]+1 << endl;
    // ## add dummy source

    sort( edges.begin(), edges.end() );

    vector<int> alpha(sigma);
    for( int i = 0; i < sigma; ++i ) { alpha[i] = i; }

    int old_u = -1;
    int alph_i = 0;
    for( int i = 0; i < edges.size(); ++i ) {
        if( old_u != edges[i].first ) {
            alph_i = 0;
        }
        swap( alpha[alph_i], alpha[alph_i+rand_range(sigma-alph_i)] );
        int label = alpha[alph_i++];
        cout << edges[i].first + 1 << ' ' << label + 1 << ' ' << edges[i].second + 1 << endl;
        old_u = edges[i].first;
    }

    sort( F.begin(), F.end() );
    for( int i = 0; i < F.size(); ++i ) {
        cout << F[i] + 1 << endl;
    }
}

void print_help( void ) {
    cout << "usage: " << endl;
    cout << " -n [num]  : number of nodes" << endl;
    cout << " -s [sigma]: alphabet size" << endl;
    cout << " -i [indeg]: indegree (except some of the first nodes)" << endl;
    cout << " -a [#edge]: create additional random transitions" << endl;
    cout << " -B        : allow backward transitions (possibly break acyclicity)" << endl;
    cout << " -S        : shuffle node ID" << endl;
    cout << " -r [seed] : random seed" << endl;
}

int main( int argc, char **argv ) {
    int opt;
    int n = 10;
    int sigma = 4;
    int indeg = 1;
    int add_edges = 0;
    bool acyclic = true;
    bool shuffle = false;
    bool debug = false;
    long long int seed = chrono::system_clock::now().time_since_epoch().count();
    while( ( opt = getopt( argc, argv, "n:s:i:a:r:BSgh" ) ) != -1 ) {
        switch( opt ) {
            case 'n': n         = atoi ( optarg ); break;
            case 's': sigma     = atoi ( optarg ); break;
            case 'i': indeg     = atoi ( optarg ); break;
            case 'a': add_edges = atoi ( optarg ); break;
            case 'r': seed      = atoll( optarg ); break;
            case 'B': acyclic = false; break;
            case 'S': shuffle = true;  break;
            case 'g': debug   = true; break;
            case 'h': print_help(); return 1;
            default:;
        }
    }
    srand( seed );
    generate( n, sigma, indeg, add_edges, acyclic, shuffle, debug );
    return 0;
}
