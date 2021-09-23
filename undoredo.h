#pragma once

#include <iostream>
#include <variant>
#include <vector>

template <typename ActionType, typename ApplyVisitor, typename UndoVisitor> class UndoRedo {
  public:
    UndoRedo() : m_last_applied(-1) {}

    void add(ActionType action) {
        m_actions.resize(m_last_applied + 1);
        m_actions.push_back(action);
    }

    void apply_all_unapplied() {
        for (int i = m_last_applied + 1; i < m_actions.size(); ++i) {
            apply(i);
        }
    }

    void undo() {
        if (m_last_applied >= 0) {
            std::visit(UndoVisitor{}, m_actions[m_last_applied]);
            m_last_applied--;
        }
    }

    void redo() {
        int size = m_actions.size(); // Needs to be declared otherwise won't work
        if (m_last_applied < size - 1) {
            apply(m_last_applied + 1);
        }
    }

  private:
    void apply(int index) {
        std::visit(ApplyVisitor{}, m_actions[index]);
        m_last_applied = index;
    }

    void show() {
        for (int i = 0; i < m_actions.size(); ++i) {
            std::cout << "[" << ((m_last_applied == i) ? "*" : " ") << "] ";
        }
        std::cout << std::endl;
    }

    int m_last_applied;
    std::vector<ActionType> m_actions;
};