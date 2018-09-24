#ifndef COMPONENT_UTILS_H
#define COMPONENT_UTILS_H

#include <memory>

#define MAX_COMPONENT 100
typedef std::bitset<MAX_COMPONENT> componentSignature;


template<class...Ts>
using operation = void(*)(void* t, void*state, Ts...);

template<class...Ts>
struct invoker{
    operation<Ts...> f;
    std::shared_ptr<void> state;
    void operator()(void* t, Ts...ts)const{
        f(t, state.get(), std::forward<Ts>(ts)...);
    }
};

template<class T, class...Ts, class F>
invoker<Ts...> make_invoker(F&& f){
    return {
        [](void* pt, void* state, Ts...ts){
            auto* pf=static_cast<std::decay_t<F>*>(state);
            (*pf)( *static_cast<T*>(pt), std::forward<Ts>(ts)... );
        },
        std::make_shared<std::decay_t<F>>( std::forward<F>(f) )
    };
}

template<class T>
invoker<int> erase_at_index(){
  return make_invoker<std::vector<T>,int>(
      []( auto&&vec, int index ){
          vec.erase(vec.begin()+index);
        }
  );
}


inline int generate_type_id() {
    static int value = 0;
    return value++;
}

template<class T>
int type_id() {
    static int value = generate_type_id();
    return value;
}

#endif