#include "structures.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <limits>
#include <iomanip>

using namespace std;

// ==========================================
// ĐỊNH NGHĨA HÀM CỦA FLASHCARD
// ==========================================
FlashCard::FlashCard(const std::string& w, const std::string& m, const std::string& ex)
    : word(w), meaning(m), example(ex), streak(0), interval(1), priority(0), correctCount(0), wrongCount(0) {
}

double FlashCard::accuracy() const {
    int total = correctCount + wrongCount;
    if (total == 0) return 0.0;
    return (double)correctCount / total * 100.0;
}

bool ComparePriority::operator()(const FlashCard& a, const FlashCard& b) const {
    return a.priority > b.priority;
}

// ==========================================
// ĐỊNH NGHĨA HÀM CỦA BST
// ==========================================
BST::Node* BST::insertNode(std::unique_ptr<Node>& node, const FlashCard& fc, bool& inserted) {
    if (!node) {
        node = std::make_unique<Node>(fc);
        inserted = true;
        return node.get();
    }
    if (fc.word < node->data.word) return insertNode(node->left, fc, inserted);
    else if (fc.word > node->data.word) return insertNode(node->right, fc, inserted);
    else {
        node->data.meaning = fc.meaning;
        node->data.example = fc.example;
        inserted = false;
        return node.get();
    }
}

BST::Node* BST::findNode(Node* node, const std::string& word) const {
    if (!node) return nullptr;
    if (word == node->data.word) return node;
    if (word < node->data.word) return findNode(node->left.get(), word);
    return findNode(node->right.get(), word);
}

std::unique_ptr<BST::Node> BST::removeNode(std::unique_ptr<Node> node, const std::string& word, bool& removed) {
    if (!node) return nullptr;
    if (word < node->data.word) node->left = removeNode(std::move(node->left), word, removed);
    else if (word > node->data.word) node->right = removeNode(std::move(node->right), word, removed);
    else {
        removed = true;
        if (!node->left) return std::move(node->right);
        if (!node->right) return std::move(node->left);
        Node* successor = node->right.get();
        while (successor->left) successor = successor->left.get();
        node->data = successor->data;
        bool dummy;
        node->right = removeNode(std::move(node->right), successor->data.word, dummy);
    }
    return node;
}

void BST::inorderCollect(Node* node, std::vector<FlashCard*>& out) const {
    if (!node) return;
    inorderCollect(node->left.get(), out);
    out.push_back(&node->data);
    inorderCollect(node->right.get(), out);
}

bool BST::insert(const FlashCard& fc) {
    bool inserted = false;
    insertNode(root, fc, inserted);
    if (inserted) count++;
    return inserted;
}

FlashCard* BST::find(const std::string& word) const {
    Node* n = findNode(root.get(), word);
    return n ? &n->data : nullptr;
}
bool BST::remove(const std::string& word) {
    bool removed = false;
    root = removeNode(std::move(root), word, removed);
    if (removed) count--;
    return removed;
}

std::vector<FlashCard*> BST::getAll() const {
    std::vector<FlashCard*> result;
    inorderCollect(root.get(), result);
    return result;
}

int BST::size() const { return count; }
bool BST::empty() const { return count == 0; }

// ==========================================
// ĐỊNH NGHĨA HÀM CỦA FLASHCARD MANAGER
// ==========================================
bool FlashcardManager::addCard(const std::string& word, const std::string& meaning, const std::string& example) {
    FlashCard fc(word, meaning, example);
    fc.priority = clock_;
    return dictionary.insert(fc);
}

bool FlashcardManager::editCard(const std::string& word, const std::string& meaning, const std::string& example) {
    FlashCard* fc = dictionary.find(word);
    if (!fc) return false;
    fc->meaning = meaning;
    fc->example = example;
    return true;
}

bool FlashcardManager::deleteCard(const std::string& word) {
    return dictionary.remove(word);
}

FlashCard* FlashcardManager::searchCard(const std::string& word) const {
    return dictionary.find(word);
}

std::vector<FlashCard*> FlashcardManager::allCards() const {
    return dictionary.getAll();
}

std::priority_queue<FlashCard, std::vector<FlashCard>, ComparePriority> FlashcardManager::buildSessionQueue() {
    std::priority_queue<FlashCard, std::vector<FlashCard>, ComparePriority> pq;
    for (FlashCard* fc : dictionary.getAll()) {
        pq.push(*fc);
    }
    return pq;
}

void FlashcardManager::recordAnswer(const std::string& word, bool isCorrect) {
    FlashCard* fc = dictionary.find(word);
    if (!fc) return;

    undoStack.push({ word, isCorrect, fc->streak, fc->interval, fc->priority });
    clock_++;

    if (isCorrect) {
        fc->correctCount++;
        fc->streak++;
        fc->interval = fc->interval * 2;
        if (fc->interval > 64) fc->interval = 64;
    }
    else {
        fc->wrongCount++;
        fc->streak = 0;
        fc->interval = 1;
    }
    fc->priority = clock_ + fc->interval;
}

bool FlashcardManager::undoLastAnswer() {
    if (undoStack.empty()) return false;
    ReviewRecord rec = undoStack.top();
    undoStack.pop();

    FlashCard* fc = dictionary.find(rec.word);
    if (!fc) return false;

    if (rec.wasCorrect) fc->correctCount--;
    else fc->wrongCount--;

    fc->streak = rec.prevStreak;
    fc->interval = rec.prevInterval;
    fc->priority = rec.prevPriority;
    if (clock_ > 0) clock_--;
    return true;
}

bool FlashcardManager::canUndo() const { return !undoStack.empty(); }

double FlashcardManager::overallAccuracy() const {
    int correct = 0, wrong = 0;
    for (FlashCard* fc : dictionary.getAll()) {
        correct += fc->correctCount;
        wrong += fc->wrongCount;
    }
    int total = correct + wrong;
    if (total == 0) return 0.0;
    return (double)correct / total * 100.0;
}
std::vector<FlashCard*> FlashcardManager::mostForgotten(int topN) const {
    std::vector<FlashCard*> cards = dictionary.getAll();
    std::sort(cards.begin(), cards.end(), [](FlashCard* a, FlashCard* b) {
        return a->wrongCount > b->wrongCount;
        });
    if ((int)cards.size() > topN) cards.resize(topN);
    return cards;
}

int FlashcardManager::totalCards() const { return dictionary.size(); }

bool FlashcardManager::saveToFile() const {
    std::ofstream out(dataFile);
    if (!out.is_open()) return false;
    out << clock_ << "\n";
    for (FlashCard* fc : dictionary.getAll()) {
        out << fc->word << "|" << fc->meaning << "|" << fc->example << "|"
            << fc->streak << "|" << fc->interval << "|" << fc->priority << "|"
            << fc->correctCount << "|" << fc->wrongCount << "\n";
    }
    out.close();
    return true;
}

bool FlashcardManager::loadFromFile() {
    std::ifstream in(dataFile);
    if (!in.is_open()) return false;
    std::string line;
    if (!std::getline(in, line)) return false;
    try { clock_ = std::stoll(line); }
    catch (...) { clock_ = 0; }

    while (std::getline(in, line)) {
        if (line.empty()) continue;
        std::vector<std::string> fields;
        std::stringstream ss(line);
        std::string token;
        while (std::getline(ss, token, '|')) fields.push_back(token);
        if (fields.size() < 8) continue;

        FlashCard fc;
        fc.word = fields[0]; fc.meaning = fields[1]; fc.example = fields[2];
        try {
            fc.streak = std::stoi(fields[3]);
            fc.interval = std::stoll(fields[4]);
            fc.priority = std::stoll(fields[5]);
            fc.correctCount = std::stoi(fields[6]);
            fc.wrongCount = std::stoi(fields[7]);
        }
        catch (...) { continue; }
        dictionary.insert(fc);
    }
    in.close();
    return true;
}

// ==========================================
// ĐỊNH NGHĨA CÁC HÀM CHỨC NĂNG 
// ==========================================
void pause() {
    cout << "\nNhan Enter de tiep tuc...";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cin.get();
}

string readLine(const string& prompt) {
    cout << prompt;
    string s;
    getline(cin, s);
    return s;
}

void printCard(const FlashCard* fc) {
    cout << "  Tu: " << fc->word << "\n"
        << "  Nghia: " << fc->meaning << "\n"
        << "  Vi du: " << fc->example << "\n"
        << "  Streak: " << fc->streak << " | Dung: " << fc->correctCount
        << " | Sai: " << fc->wrongCount
        << " | Ty le dung: " << fixed << setprecision(1) << fc->accuracy() << "%\n";
}

void menuAddCard(FlashcardManager& mgr) {
    string w = readLine("Tu vung: ");
    if (mgr.searchCard(w)) {
        cout << "Tu nay da ton tai! Dung chuc nang Sua neu muon cap nhat.\n";
        return;
    }
    string m = readLine("Nghia: ");
    string e = readLine("Vi du: ");
    mgr.addCard(w, m, e);
    cout << "Da them flashcard moi.\n";
}
void menuEditCard(FlashcardManager& mgr) {
    string w = readLine("Tu can sua: ");
    if (!mgr.searchCard(w)) {
        cout << "Khong tim thay tu nay.\n";
        return;
    }
    string m = readLine("Nghia moi: ");
    string e = readLine("Vi du moi: ");
    mgr.editCard(w, m, e);
    cout << "Da cap nhat.\n";
}

void menuDeleteCard(FlashcardManager& mgr) {
    string w = readLine("Tu can xoa: ");
    if (mgr.deleteCard(w)) cout << "Da xoa.\n";
    else cout << "Khong tim thay tu nay.\n";
}

void menuSearch(FlashcardManager& mgr) {
    string w = readLine("Nhap tu can tim: ");
    FlashCard* fc = mgr.searchCard(w);
    if (!fc) { cout << "Khong tim thay.\n"; return; }
    printCard(fc);
}

void menuListAll(FlashcardManager& mgr) {
    auto cards = mgr.allCards();
    if (cards.empty()) { cout << "Tu dien dang trong.\n"; return; }
    cout << "Co " << cards.size() << " tu trong tu dien (sap theo alphabet):\n\n";
    for (auto* fc : cards) {
        printCard(fc);
        cout << "  ----------\n";
    }
}


        void menuStudySession(FlashcardManager & mgr) {
        if (mgr.totalCards() == 0) {
            cout << "Chua co flashcard nao, hay them tu truoc.\n";
            return;
        }
        int numQuestions;
            cout << "Hoc bao nhieu cau hoi trong phien nay? ";
            cin >> numQuestions;
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            if (numQuestions <= 0) numQuestions = 5;

            auto pq = mgr.buildSessionQueue();
        int asked = 0;
        while (asked < numQuestions && !pq.empty()) {
            FlashCard top = pq.top();
            pq.pop();

            cout << "\n[Cau " << (asked + 1) << "] Tu: " << top.word << "\n";
            string answer = readLine("Nhap nghia cua tu nay: ");

            cout << "Nghia dung: " << top.meaning << "\n";
            cout << "Vi du: " << top.example << "\n";
            string judge = readLine("Ban tra loi dung hay sai? (d/s): ");
            bool correct = (!judge.empty() && (judge[0] == 'd' || judge[0] == 'D'));
            mgr.recordAnswer(top.word, correct);

            FlashCard* updated = mgr.searchCard(top.word);
            if (updated && !correct) pq.push(*updated);

            if (correct) cout << ">> Chinh xac!\n";
            else cout << ">> Sai roi, tu nay se duoc uu tien on lai som hon.\n";
            asked++;
        }
        cout << "\nKet thuc phien hoc! Da on " << asked << " cau.\n";
    }

    void menuStats(FlashcardManager & mgr) {
        cout << fixed << setprecision(1);
        cout << "Tong so flashcard: " << mgr.totalCards() << "\n";
        cout << "Ty le dung tong the: " << mgr.overallAccuracy() << "%\n\n";

        auto forgotten = mgr.mostForgotten(5);
        if (forgotten.empty()) {
            cout << "Chua co du lieu de thong ke tu hay quen.\n";
            return;
        }
        cout << "Top tu hay quen nhat:\n";
        int rank = 1;
        for (auto* fc : forgotten) {
            if (fc->wrongCount == 0) continue;
            cout << "  " << rank++ << ". " << fc->word << " - sai " << fc->wrongCount
                << " lan (ty le dung rieng: " << fc->accuracy() << "%)\n";
        }
    }

    void menuUndo(FlashcardManager & mgr) {
        if (mgr.undoLastAnswer()) cout << "Da hoan tac lan tra loi gan nhat.\n";
        else cout << "Khong co gi de hoan tac.\n";
    }

    void showMenu() {
        cout << "\n========= APP HOC TU VUNG (SPACED REPETITION) =========\n";
        cout << "1. Them flashcard moi\n";
        cout << "2. Sua flashcard\n";
        cout << "3. Xoa flashcard\n";
        cout << "4. Bat dau phien hoc (Spaced Repetition)\n";
        cout << "5. Tim kiem tu (BST)\n";
        cout << "6. Xem toan bo tu dien\n";
        cout << "7. Thong ke\n";
        cout << "8. Hoan tac (Undo) lan tra loi gan nhat\n";
        cout << "9. Luu tien do ra file\n";
        cout << "0. Thoat (tu dong luu)\n";
        cout << "Chon: ";
    }
