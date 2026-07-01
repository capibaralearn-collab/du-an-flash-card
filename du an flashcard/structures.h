#ifndef STRUCTURES_H
#define STRUCTURES_H

#include <string>
#include <vector>
#include <memory>
#include <queue>
#include <stack>
#include <algorithm>

// ---------- 1. Cấu trúc FlashCard ----------
struct FlashCard {
    std::string word;
    std::string meaning;
    std::string example;

    int streak = 0;
    long long interval = 1;
    long long priority = 0;

    int correctCount = 0;
    int wrongCount = 0;

    FlashCard() = default;
    FlashCard(const std::string& w, const std::string& m, const std::string& ex);

    double accuracy() const;
};

// Comparator để ưu tiên FlashCard cần ôn sớm nhất lên đầu
struct ComparePriority {
    bool operator()(const FlashCard& a, const FlashCard& b) const;
};

// ---------- 2. Cấu trúc Cây Nhị Phân (BST) ----------
class BST {
private:
    struct Node {
        FlashCard data;
        std::unique_ptr<Node> left;
        std::unique_ptr<Node> right;
        Node(const FlashCard& fc) : data(fc) {}
    };

    std::unique_ptr<Node> root;
    int count = 0;

    Node* insertNode(std::unique_ptr<Node>& node, const FlashCard& fc, bool& inserted);
    Node* findNode(Node* node, const std::string& word) const;
    std::unique_ptr<Node> removeNode(std::unique_ptr<Node> node, const std::string& word, bool& removed);
    void inorderCollect(Node* node, std::vector<FlashCard*>& out) const;

public:
    bool insert(const FlashCard& fc);
    FlashCard* find(const std::string& word) const;
    bool remove(const std::string& word);
    std::vector<FlashCard*> getAll() const;
    int size() const;
    bool empty() const;
};

// ---------- 3. Cấu trúc FlashcardManager ----------
struct ReviewRecord {
    std::string word;
    bool wasCorrect;
    int prevStreak;
    long long prevInterval;
    long long prevPriority;
};

class FlashcardManager {
private:
    BST dictionary;
    long long clock_ = 0;
    std::stack<ReviewRecord> undoStack;
    const std::string dataFile = "flashcards.txt";

public:
    bool addCard(const std::string& word, const std::string& meaning, const std::string& example);
    bool editCard(const std::string& word, const std::string& meaning, const std::string& example);
    bool deleteCard(const std::string& word);

    FlashCard* searchCard(const std::string& word) const;
    std::vector<FlashCard*> allCards() const;

    std::priority_queue<FlashCard, std::vector<FlashCard>, ComparePriority> buildSessionQueue();
    void recordAnswer(const std::string& word, bool isCorrect);

    bool undoLastAnswer();
    bool canUndo() const;

    double overallAccuracy() const;
    std::vector<FlashCard*> mostForgotten(int topN) const;
    int totalCards() const;

    bool saveToFile() const;
    bool loadFromFile();
};




// ============================================================================
// 4. Khai báo các nguyên mẫu hàm 
// ============================================================================

void pause();
void showMenu();
void menuAddCard(FlashcardManager& mgr);
void menuEditCard(FlashcardManager& mgr);
void menuDeleteCard(FlashcardManager& mgr);
void menuSearch(FlashcardManager& mgr);
void menuListAll(FlashcardManager& mgr);
void menuStudySession(FlashcardManager& mgr);
void menuStats(FlashcardManager& mgr);
void menuUndo(FlashcardManager& mgr);




#endif