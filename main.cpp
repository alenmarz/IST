#include <iostream>
#include <set>
#include <chrono>
#include <memory>
#include <cstdlib>
#include <random>
#include <stdlib.h>
#include "Treap.h"
#include "Tree.h"
#include <time.h>

void correctnessTest() {
    std::set<int> set;
    Tree<int> tree;

    for (int digit = 0; digit < 100000; digit++) {
        if (rand() % 2) {
            tree.insert(std::make_shared<Element<int>>(digit, digit));
            set.insert(digit);
        }
    }

    tree.rebuild();
    tree.print("");


    while (true) {
        int digit = rand() % 100000;
        switch (rand() % 3) {
            case 0:
                if (set.insert(digit).second != tree.insert(std::make_shared<Element<int>>(digit, digit))) {
                    std::cout << "Insert Error!" << std::endl;
                    tree.getSize();
                }
                break;
            case 1:
                if (set.erase(digit) != tree.remove(digit)) {
                    std::cout << "Remove Error!" << std::endl;
                    tree.getSize();
                }
                break;
            case 2:
                bool setContains = set.find(digit) != set.end();
                if (setContains != tree.contains(digit)) {
                    std::cout << "Contains Error!" << tree.getSize() << std::endl;
                }
                break;
        }
    }
}


void test31() {
    Treap<int> treap;

    srand(3);
    for (int digit = 0; digit < 10000000; digit++) {
        if (rand() % 2) {
            treap.insert(std::make_shared<Element<int>>(digit, digit));
        }
    }

    auto vect = std::make_shared<Actions<int>>();

    srand(2);
    for (int i = 0; i < 10000000; i++) {
        int a = rand();
        switch (a % 3) {
            case 0:
                vect->push_back(std::make_shared<Action<int>>(std::make_shared<Element<int>>(i, i), Insert));
                break;
            case 1:
                vect->push_back(std::make_shared<Action<int>>(std::make_shared<Element<int>>(i, i), Remove));
                break;
            case 2:
                vect->push_back(std::make_shared<Action<int>>(std::make_shared<Element<int>>(i, i), Contains));
                break;
        }
    }

    auto start = std::chrono::high_resolution_clock::now();
    treap.p_execute(vect);
    auto finish = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = finish - start;
    std::cout << "exectime: " << elapsed.count() << std::endl;
}

void test32() {
    Tree<int> tree;

    srand(3);
    for (int digit = 0; digit < 20; digit++) {
        if (rand() % 2) {
            tree.insert(std::make_shared<Element<int>>(digit, digit));
        }
    }

	tree.print("");

    auto vect = std::make_shared<Actions<int>>();

    srand(2);
    for (int i = 0; i < 20; i++) {
        int a = rand();
        switch (a % 3) {
            case 0:
		std::cout << "Insert " << i << std::endl;
                vect->push_back(std::make_shared<Action<int>>(std::make_shared<Element<int>>(i, i), Insert, i));
                break;
            case 1:
		std::cout << "Remove " << i << std::endl;
                vect->push_back(std::make_shared<Action<int>>(std::make_shared<Element<int>>(i, i), Remove, i));
                break;
            case 2:
		std::cout << "Contains " << i << std::endl;
                vect->push_back(std::make_shared<Action<int>>(std::make_shared<Element<int>>(i, i), Contains, i));
                break;
        }
        rand();
    }

    auto start = std::chrono::high_resolution_clock::now();
    auto res = tree.p_execute(vect);
    for (auto e: *res) {
	std::cout << e << " ";
	   }
    std::cout << std::endl;
    auto finish = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = finish - start;

    tree.print("");

    std::cout << "exectime: " << elapsed.count() << std::endl;
}

void test33(int N) {

    Tree<int> tree;
	Treap<int> treap;

 	srand(3);
    for (int digit = 0; digit < N; digit++) {
        if (rand() % 2) {
            tree.insert(std::make_shared<Element<int>>(digit, digit));
	    treap.insert(std::make_shared<Element<int>>(digit, digit));
        }
    }

    auto vect = std::make_shared<Actions<int>>();
    auto vect1 = std::make_shared<Actions<int>>();

    srand(1);
    for (int i = 0; i < N; i++) {
        int a = rand();
        switch (a % 3) {
            case 0:
          //      std::cout << "Insert " << i << std::endl;
                vect->push_back(std::make_shared<Action<int>>(std::make_shared<Element<int>>(i, i), Insert, i));
                vect1->push_back(std::make_shared<Action<int>>(std::make_shared<Element<int>>(i, i), Insert, i));
		break;
            case 1:
            //    std::cout << "Remove " << i << std::endl;
                vect->push_back(std::make_shared<Action<int>>(std::make_shared<Element<int>>(i, i), Remove, i));
                vect1->push_back(std::make_shared<Action<int>>(std::make_shared<Element<int>>(i, i), Remove, i));
		break;
            case 2:
              //  std::cout << "Contains " << i << std::endl;
                vect->push_back(std::make_shared<Action<int>>(std::make_shared<Element<int>>(i, i), Contains, i));
                vect1->push_back(std::make_shared<Action<int>>(std::make_shared<Element<int>>(i, i), Contains, i));
		break;
        }
    }

    auto start = std::chrono::high_resolution_clock::now();
    auto res = tree.p_execute(vect);
    auto finish = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = finish - start;

/*	for (auto e: *res) {
        	std::cout << e << " ";
        }
	*/
    //std::cout << std::endl;

    //tree.print("");
    //std::cout << "-------------" << std::endl;
	std::cout << "exectime (tree): " << elapsed.count() << std::endl;
    start = std::chrono::high_resolution_clock::now();
    auto result = treap.p_execute(vect);
    finish = std::chrono::high_resolution_clock::now();
    elapsed = finish - start;

    std::cout << "exectime (treap): " << elapsed.count() << std::endl;

//	treap.print("");
   
       srand(1);
    for (int i = 0; i < N; i++) {
        switch (rand() % 3) {
            case 0:
                if (res->at(i) != result->at(i)) {
                    std::cout << "Insert Error! " << res->at(i) << " " << result->at(i) << " " << i << std::endl;
                }
                break;
            case 1:
                if (res->at(i) != result->at(i)) {
                    std::cout << "Remove Error! " << res->at(i) << " " << result->at(i) << " " << i << std::endl;
                }
                break;
            case 2:
                if (res->at(i) != result->at(i)) {
                    std::cout << "Contains Error! " << res->at(i) << " " << result->at(i) << " " << i << std::endl;
                }
                break;
        }
	rand(); rand();
    }
}

void correctnessTest1() {
    //std::set<int> set;

    Treap<int> treap;
    Treap<int> tree;

    /*for (int digit = 0; digit < 10; digit++) {
        if (rand() % 2) {
            tree.insert(std::make_shared<Element<int>>(digit, digit));
            treap.insert(std::make_shared<Element<int>>(digit, digit));
            //set.insert(digit);
        }
    }*/

    //tree.print("");
    //std::cout << "---------" << std::endl;


    auto res = std::make_shared<std::vector<bool>>();
    //auto v = std::make_shared<Actions<int>>();
    auto vect = std::make_shared<Actions<int>>();
    auto start = std::chrono::high_resolution_clock::now();
    //srand(1);
    for (int i = 0; i < 10; i++) {
        int digit = i;
        //std::cout << digit << std::endl;
        switch (0) {
            case 0:
                //v->push_back(std::make_shared<Action<int>>(std::make_shared<Element<int>>(i, i), Insert));
                res->push_back(treap.insert(std::make_shared<Element<int>>(i, i)));
                break;
            case 1:
                //res->push_back(treap.remove(i));
                //vect->push_back(std::make_shared<Action<int>>(std::make_shared<Element<int>>(i, i), Remove));
                break;
            case 2:
                //res->push_back(treap.contains(i));
                //vect->push_back(std::make_shared<Action<int>>(std::make_shared<Element<int>>(i, i), Contains));
                break;
        }
    }
    auto finish = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = finish - start;
    std::cout << "Time for seq: " << elapsed.count() << std::endl;

    //treap.print("");
    //std::cout << "---------" << std::endl;

    //srand(1);
        for (int i = 0; i < 10; i++) {
            int digit = i;
            //std::cout << digit << std::endl;
            switch (0) {
                case 0:
                    vect->push_back(std::make_shared<Action<int>>(std::make_shared<Element<int>>(i, i), Insert));
                    //res->push_back(set.insert(i).second);
                    break;
                case 1:
                    //res->push_back(set.erase(i));
                    //vect->push_back(std::make_shared<Action<int>>(std::make_shared<Element<int>>(i, i), Remove));
                    break;
                case 2:
                    //res->push_back(set.find(i) != set.end());
                    //vect->push_back(std::make_shared<Action<int>>(std::make_shared<Element<int>>(i, i), Contains));
                    break;
            }
        }

    start = std::chrono::high_resolution_clock::now();
    auto result = tree.p_execute(vect);
    finish = std::chrono::high_resolution_clock::now();
    elapsed = finish - start;
    std::cout << "Time for par: " << elapsed.count() << std::endl;

    //tree.print("");
    //tree.print("");
    /*for (auto a: *result) {
        std::cout << a << " ";
    }
    std::cout << std::endl;*/

    /*srand(1);
    for (int i = 0; i < 10000000; i++) {
        switch (rand() % 3) {
            case 0:
                if (res->at(i) != result->at(i)) {
                    std::cout << "Insert Error! " << res->at(i) << " " << result->at(i) << " " << i << std::endl;
                }
                break;
            case 1:
                if (res->at(i) != result->at(i)) {
                    std::cout << "Insert Error! " << res->at(i) << " " << result->at(i) << " " << i << std::endl;
                }
                break;
            case 2:
                if (res->at(i) != result->at(i)) {
                    std::cout << "Insert Error! " << res->at(i) << " " << result->at(i) << " " << i << std::endl;
                }
                break;
        }
    }*/
}

void test1() {
    Treap<int> treap;

    for (int digit = 0; digit < 100; digit++) {
        std::cout << digit << " " << treap.insert(std::make_shared<Element<int>>(digit, digit)) << std::endl;
    }

    treap.print("");
}

void test() {
    std::set<int> set;
    Tree<int> tree;

    for (int digit = 0; digit < 5000000; digit++) {
        if (rand() % 2) {
            tree.insert(std::make_shared<Element<int>>(digit, digit));
            set.insert(digit);
        }
    }

    //tree.rebuild();
    int max_depth = 0;
    long total_depth = 0;

    srand(1);
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < 10000000; i++) {
        int digit = rand() % 5000000;
        //int depth = 0;
        //tree.contains(digit, &depth);
        tree.contains(digit);
        //total_depth += depth;
        //max_depth = std::max(max_depth, depth);
        /*switch (rand() % 3) {
            case 0:
                tree.insert(std::make_shared<Element<int>>(digit, digit));
                //std::cout<<"insert" <<std::endl;
                break;
            case 1:
                tree.remove(digit);
                //std::cout<<"remove" <<std::endl;
                break;
            case 2:
                tree.contains(digit);
                //std::cout<<"contains" <<std::endl;
                break;
        }*/
    }

    auto finish = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = finish - start;
    std::cout << "Time for tree: " << elapsed.count() << std::endl;
    //std::cout << "max depth: " << max_depth << std::endl;
    //std::cout << "avg: " << total_depth / 10000000 << std::endl;

    //tree.print("");

    srand(1);
    start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < 10000000; i++) {
        int digit = rand() % 5000000;
        set.find(digit);
        /*switch (rand() % 3) {
        //switch (0) {
            case 0:
                set.insert(digit);
                break;
            case 1:
                set.erase(digit);
                break;
            case 2:
                set.find(digit);
                break;
        }*/
    }

    finish = std::chrono::high_resolution_clock::now();
    elapsed = finish - start;
    std::cout << "Time for set: " << elapsed.count() << std::endl;
}

void test2() {
    Treap<int> treap;
    Tree<int> tree;

    /*for (int digit = 0; digit < 5000000; digit++) {
        if (rand() % 2) {
            tree.insert(std::make_shared<Element<int>>(digit, digit));
            treap.insert(std::make_shared<Element<int>>(digit, digit));
        }
    }*/

    //tree.rebuild();
    int max_depth = 0;
    long total_depth = 0;

    srand(1);
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < 10000000; i++) {
        int digit = rand() % 5000000;
        //int depth = 0;
        //tree.contains(digit, &depth);
        //tree.contains(digit);
        //total_depth += depth;
        //max_depth = std::max(max_depth, depth);
        switch (rand() % 3) {
            case 0:
                tree.insert(std::make_shared<Element<int>>(digit, digit));
                //std::cout<<"insert" <<std::endl;
                break;
            case 1:
                tree.remove(digit);
                //std::cout<<"remove" <<std::endl;
                break;
            case 2:
                tree.contains(digit);
                //std::cout<<"contains" <<std::endl;
                break;
        }
    }

    auto finish = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = finish - start;
    std::cout << "Time for tree: " << elapsed.count() << std::endl;
    //std::cout << "max depth: " << max_depth << std::endl;
    //std::cout << "avg: " << total_depth / 10000000 << std::endl;

    //tree.print("");

    srand(1);
    start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < 10000000; i++) {
        int digit = rand() % 5000000;
        //treap.contains(digit);
        switch (rand() % 3) {
            case 0:
                treap.insert(std::make_shared<Element<int>>(digit, digit));
                //std::cout<<"insert" <<std::endl;
                break;
            case 1:
                treap.remove(digit);
                //std::cout<<"remove" <<std::endl;
                break;
            case 2:
                treap.contains(digit);
                //std::cout<<"contains" <<std::endl;
                break;
        }
    }

    finish = std::chrono::high_resolution_clock::now();
    elapsed = finish - start;
    std::cout << "Time for treap: " << elapsed.count() << std::endl;
}


int main(int argc, char** argv) {
	pbbs::launch(argc, argv, [&] (pbbs::measured_type measure) {
		test33(1000);
	});
    //correctnessTest1();
    /*auto start = std::chrono::high_resolution_clock::now();
    fib_seq(100);
    auto finish = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = finish - start;
    std::cout << "Time for seq: " << elapsed.count() << std::endl;
    start = std::chrono::high_resolution_clock::now();
    finish = std::chrono::high_resolution_clock::now();
    elapsed = finish - start;
    std::cout << "Time for par: " << elapsed.count() << std::endl;*/
    return 0;
}
