#include <vector>
#include <iostream>
#include <fstream>
#include <cassert>

using namespace std;

int N;

vector<int> In; // In[v]=1 iff v has any incoming transition
vector<int> R;  // R[v]: Rank of v
vector<int> R_;
vector<int> P;  // P[v]: Pointer to an extender of v
vector<int> P_;
vector<int> C;  // Array for count sort
vector<int> I;  // Permutation of {1,..,N} for sorting
vector<int> I_;

void check_size( int u ) {
    if( u < R.size() ) return;
    if( u >= R.capacity() ) {
        int c = 2*R.capacity();
        In.reserve(c);
        R .reserve(c);
        R_.reserve(c);
        P .reserve(c);
        P_.reserve(c);
        C .reserve(c);
        I .reserve(c);
        I_.reserve(c);
    }
    N = u+1;
    In.resize(N);
    R .resize(N);
    R_.resize(N);
    P .resize(N);
    P_.resize(N);
    C .resize(N);
    I .resize(N);
    I_.resize(N);
}

void sort_by_extender_rank( void ) {
    int cmax = 0;
    for( int v = 0; v < N; ++v ) {
        int r_p = R[P[v]];
        cmax = (cmax>r_p?cmax:r_p);
        C[r_p]++;
    }
    for( int i = 1; i <= cmax; ++i ) {
        C[i] += C[i-1];
    }
    for( int v = N-1; v >= 0; --v ) {
        int r_p = R[P[v]];
        I[--C[r_p]] = v;
    }
    for( int i = 0; i <= cmax; ++i ) {
        C[i] = 0;
    }
}

void sort_by_rank( void ) {
    int cmax = 0;
    for( int i = 0; i < N; ++i ) {
        int v = I[i];
        int r_p = R[v];
        cmax = (cmax>r_p?cmax:r_p);
        C[r_p]++;
    }
    for( int i = 1; i <= cmax; ++i ) {
        C[i] += C[i-1];
    }
    for( int i = N-1; i >= 0; --i ) {
        int v = I[i];
        int r_p = R[v];
        I_[--C[r_p]] = v;
    }
    for( int i = 0; i <= cmax; ++i ) {
        C[i] = 0;
    }
    swap( I, I_ );
}

void get_rank( void ) {
    int r = 0;
    swap( R_, R );
    R[0]=0;
    for( int i = 0; i < N-1; ++i ) {
        int u = I[i];
        int v = I[i+1];
        if( R_[u] < R_[v] || R_[P[u]] < R_[P[v]] ) ++r;
        R[v] = r;
    }
}

void update_pointers( void ) {
    P_.clear();
    for( int v = 0; v < N; ++v ) {
        P_[v] = P[P[v]];
    }
    swap( P, P_ );
}

void do_doubling( void ) {
    sort_by_extender_rank();
    sort_by_rank();
    get_rank();
    update_pointers();
}

int get_char_id( char a ) {
    switch( a ) {
        case 'a': return 1;
        case 'c': return 2;
        case 'g': return 3;
        case 't': return 4;
        default : return 5;
    }
}

void load_input( istream& in ) {
    int offset = N;
    int u_, v_;
    char a_;
    int max_a = 0;
    while( in >> u_ >> a_ >> v_ ) {
        int u = u_ + offset;
        int v = v_ + offset;
        //int a = get_char_id( a_ );
        int a = (int)a_;

        max_a = max( max_a, a );
        check_size( max( u, v ) );

        In[v] = 1;
        R [v] = a;
        P [v] = u;
    }

    if( max_a >= N ) C.resize( max_a + 1 );
}

int main( int argc, char **argv ) {
    {
        ifstream in_min( argv[1] ); // pruned graph for infima
        ifstream in_max( argv[2] ); // pruned graph for suprema
        load_input( in_min );
        load_input( in_max );
        assert( N%2 == 0 );
        for( int v = 0; v < N; ++v ) {
            if( !In[v] ) {
                R[v] = 0;
                P[v] = v;
            }
        }
    }

    int l = 1;
    while( l < N ) {
        do_doubling();
        l <<= 1;
    }
    N /= 2;
    for( int i = 0; i < N; ++i ) {
        int v_min =   i;
        int v_max = N+i;
        cout << R[v_min] << '\t' << R[v_max] << endl;
    }

    return 0;
}
