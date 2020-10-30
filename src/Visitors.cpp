#include <Visitors.h>
using namespace std;

void ShrinkOneChildBranches::Visit(unique_ptr<UnaryExpr>& e) {
  if (!e->is_neg())
    e.reset(e->term().release());
}

void ShrinkOneChildBranches::Visit(unique_ptr<BinaryExpr>& e) {
  for (auto& term : e->terms())
    Visit(term);
  if (e->ops().empty())
    e.reset(e->terms().back().release());
}

void ShrinkOneChildBranches::Visit(unique_ptr<Program>& e) {
  for (auto& instruction : e->instructions())
    Visit(instruction);
  if (e->instructions().size() == 1)
    e.reset(e->instructions().back().release());
}
