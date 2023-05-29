#include <iostream>
#include <vector>
#include <map>
#include <cassert>
#include <algorithm>

//#define DEBUG

void print_vec(auto v)
{
    for (auto elem : v)
        std::cout << elem << ' ';
    std::cout << '\n';
}

enum class Mark
{
    one,
    two,
    empty,
};

enum class Player
{
    one,
    two,
};

enum class Game_result
{
    running,
    one,
    two,
    tie,
};

std::map<Mark, char> symbols{
    { Mark::one, 'X' },
    { Mark::two, '0' },
    { Mark::empty, ' ' } };

std::ostream& operator<<(std::ostream& os, const Mark mark)
{
    return os << symbols[mark];
}

using Line_pos = std::vector<int>;

class Grid
{
public:
    Grid(int size);
    void display() const;
    Game_result state() const;
    void update(Player& player);

private:
    int m_size;
    std::vector<Mark> m_marks;
    std::vector<Line_pos> m_lines_pos;

    int get_pos(Player player);
};

Grid::Grid(int size) : m_size{ size }
{
    for (int i = 0; i < size * size; ++i)
        m_marks.push_back(Mark::empty);

    // horizontal and vertical lines
    for (int i = 0; i < size; ++i) {
        Line_pos h_pos;
        Line_pos v_pos;
        for (int j = 0; j < size; ++j) {
            h_pos.push_back(i * size + j);
            v_pos.push_back(j * size + i);
        }
        m_lines_pos.push_back(h_pos);
        m_lines_pos.push_back(v_pos);
    }

    // diagonal lines
    Line_pos first{ 0 };
    Line_pos second{ size - 1 };
    for (int i = 0; i < size - 1; ++i) {
        first.push_back(first.back() + size + 1);
        second.push_back(second.back() + size - 1);
    }

    m_lines_pos.push_back(first);
    m_lines_pos.push_back(second);

#ifdef DEBUG
    for (auto lines : m_lines_pos)
        print_vec(lines);
#endif
}

void Grid::display() const
{
    auto print_line = [this]() -> void
    {
        for (int i = 0; i < m_size; ++i)
            std::cout << "----";
        std::cout << "-\n";
    };

    print_line();

    for (std::size_t i = 0; i < std::size(m_marks); ++i) {
        std::cout << "| " << symbols[m_marks[i]] << ' ';
        if ((i + 1) % m_size == 0) {
            std::cout << "|\n";
            print_line();
        }
    }
}

void switch_player(Player&);

void Grid::update(Player& player)
{
    while (true) {
        int pos = get_pos(player);
        if (m_marks[pos] == Mark::empty) {
            m_marks[pos] = static_cast<Mark>(player);
            switch_player(player);
            return;
        }
        std::cout << "This position is already played.\n";
    }
}

Game_result Grid::state() const
{
    // check if a player won
    for (auto lines_pos : m_lines_pos) {
#ifdef DEBUG
        std::cout << "Line pos: ";
        print_vec(lines_pos);
#endif

        std::vector<Mark> values;
        for (auto pos : lines_pos)
            values.push_back(m_marks[pos]);

#ifdef DEBUG
        std::cout << "Line values: ";
        print_vec(values);
#endif

#ifdef DEBUG
        auto n_one = std::count(values.begin(), values.end(), Mark::one);
        auto n_two = std::count(values.begin(), values.end(), Mark::two);
        auto n_empty = std::count(values.begin(), values.end(), Mark::empty);

        std::cout << "n_one: " << n_one << '\n';
        std::cout << "n_two: " << n_two << '\n';
        std::cout << "n_empty: " << n_empty << '\n';
#endif

        if (std::count(values.begin(), values.end(), Mark::one) == m_size)
            return Game_result::one;
        else if (std::count(values.begin(), values.end(), Mark::two) == m_size)
            return Game_result::two;
    }

    // grid is full and no one won
    if (std::count(m_marks.begin(), m_marks.end(), Mark::empty) == 0)
        return Game_result::tie;

    return Game_result::running;
}

void prompt(Player player);

void ignore_line()
{
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

int Grid::get_pos(Player player)
{
    int i;
    while (true) {
        prompt(player);
        std::cin >> i;
        if (!std::cin) {
            std::cout << "Please enter a number.\n";
            std::cin.clear();
            ignore_line();
        }
        else if (i < 0 || i >= m_size * m_size) {
            std::cout << "Please enter a number in the valid range"
                " (0-" << m_size * m_size - 1 << ").\n";
            ignore_line();
        }
        else {
            ignore_line();
            return i;
        }
    }
}

void prompt(Player player)
{
    std::cout << "\tPlayer " << static_cast<int>(player) + 1 << ": ";
}

void switch_player(Player& player)
{
    if (player == Player::one) {
        player = Player::two;
        return;
    }
    player = Player::one;
}

void print_results(Game_result results)
{
    switch (results) {
    case Game_result::one:
        std::cout << "Player 1 wins!\n";
        return;
    case Game_result::two:
        std::cout << "Player 2 wins!\n";
        return;
    case Game_result::tie:
        std::cout << "Tie!\n";
        return;
    default:
        std::cout << "??\n";
        return;
    }
}

int main()
{
    std::cout << "~~~~~~~TIC-TAC-TOE~~~~~~~\n";

    Grid grid{ 3 };
    grid.display();
    Player player{ Player::one };
    while (grid.state() == Game_result::running) {
        grid.update(player);
        grid.display();
    }

    print_results(grid.state());

    return 0;
}