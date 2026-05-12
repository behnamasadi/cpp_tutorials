// Demo for docs/copy_elision.md
//
// Each Widget operation prints, so the output literally shows how many
// copy/move ctors actually ran. Fewer prints than you'd "expect" means
// the compiler elided.
//
// Build (from build/): cmake --build . --target RVO_NRVO_copy_elision -j 4
// Run:                 ./Debug/RVO_NRVO_copy_elision
//
// To see what elision is hiding, recompile with:
//     g++ -std=c++17 -fno-elide-constructors ...
// Mandatory C++17 cases (RVO from prvalues) still elide; NRVO does not.

#include <iostream>
#include <utility>

struct Widget {
    Widget()                       { std::cout << "  default\n"; }
    Widget(const Widget&)          { std::cout << "  copy\n"; }
    Widget(Widget&&) noexcept      { std::cout << "  move\n"; }
    ~Widget()                      { std::cout << "  ~Widget\n"; }
};

// --- §3 RVO: return a prvalue ---------------------------------------------
Widget rvo_prvalue() {
    return Widget{};                       // prvalue — C++17 mandatory elision
}

// --- §3 RVO: conditional prvalues -----------------------------------------
Widget rvo_conditional(bool flag) {
    return flag ? Widget{} : Widget{};     // both arms are prvalues
}

// --- §4 NRVO: named local --------------------------------------------------
Widget nrvo_named() {
    Widget local;
    return local;                          // permitted (not mandatory)
}

// --- §4.1 NRVO failure: multiple named locals on different paths ----------
Widget nrvo_fails_branch(bool flag) {
    Widget a, b;
    return flag ? a : b;                   // compiler can't pre-pick a slot
}

// --- §4.1 NRVO failure: returning a function parameter --------------------
Widget nrvo_fails_param(Widget src) {
    return src;                            // src lives in the parameter slot
}

// --- §4.2 The `return std::move(local)` anti-pattern ----------------------
Widget pessimizing_move() {
    Widget local;
    return std::move(local);               // disables NRVO, forces a move
}

// --- §5.1 C++17 mandatory elision: immovable type returned by prvalue -----
struct Immovable {
    int value;
    Immovable() : value(42)                { std::cout << "  Immovable default\n"; }
    Immovable(const Immovable&)            = delete;
    Immovable(Immovable&&)                 = delete;
    Immovable& operator=(const Immovable&) = delete;
    Immovable& operator=(Immovable&&)      = delete;
    ~Immovable()                           { std::cout << "  ~Immovable\n"; }
};

Immovable make_immovable() {
    return Immovable{};                    // legal in C++17 — no ctor call needed
}

// --- §6 Copy elision in exception handling --------------------------------
void throw_and_catch() {
    try {
        throw Widget{};                    // prvalue exception object
    } catch (const Widget&) {              // catch-by-reference: no copy
        std::cout << "  (caught by reference)\n";
    }
}

// --------------------------------------------------------------------------
static void header(const char* title) {
    std::cout << "\n=== " << title << " ===\n";
}

int main() {
    header("1. RVO from prvalue return  (expect: default + ~Widget)");
    {
        Widget w = rvo_prvalue();
    }

    header("2. RVO with conditional prvalues  (expect: default + ~Widget)");
    {
        Widget w = rvo_conditional(true);
    }

    header("3. NRVO with named local  (expect: default + ~Widget)");
    {
        Widget w = nrvo_named();
    }

    header("4. NRVO fails: branched named locals  (expect: 2 defaults + copy/move + 3 dtors)");
    {
        Widget w = nrvo_fails_branch(true);
    }

    header("5. NRVO fails: returning a parameter  (expect: default + move + dtors)");
    {
        Widget w = nrvo_fails_param(Widget{});
    }

    header("6. Anti-pattern: return std::move(local)  (expect: default + move + dtors)");
    {
        Widget w = pessimizing_move();
    }

    header("7. C++17 mandatory elision of immovable prvalue  (expect: 1 default + 1 dtor)");
    {
        Immovable x = make_immovable();
        std::cout << "  value = " << x.value << "\n";
    }

    header("8. Exception: throw prvalue, catch by ref  (expect: default + ~Widget)");
    throw_and_catch();

    std::cout << "\n=== done ===\n";
}
