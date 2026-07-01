## Tên ứng dụng
**Ứng dụng học từ vựng Flashcard thông minh (Spaced Repetition)**
Đây là ứng dụng quản lý và học từ vựng bằng dòng lệnh (CLI) áp dụng phương pháp **Lặp lại ngắt quãng (Spaced Repetition)**. Ứng dụng giúp người dùng tối ưu hóa khả năng ghi nhớ từ mới bằng cách tự động tính toán và sắp xếp lịch trình ôn tập hợp lý dựa trên lịch sử trả lời đúng/sai của từng từ vựng.

## Cấu trúc dữ liệu sử dụng
- **BST (Binary Search Tree - Cây nhị phân tìm kiếm):** - *Dùng để:* Lưu trữ toàn bộ từ điển flashcard và thực hiện các thao tác quản lý cốt lõi (thêm mới, chỉnh sửa thông tin, xóa từ vựng, tìm kiếm nhanh).
  - *Vì:* BST giúp tối ưu thời gian tìm kiếm, chèn và xóa phần tử với độ phức tạp trung bình là $O(\log n)$. Ngoài ra, việc duyệt cây theo thứ tự `Inorder` giúp xuất ra danh sách từ vựng tự động sắp xếp theo đúng thứ tự Alphabet (A-Z) một cách dễ dàng mà không cần tốn thêm chi phí chạy thuật toán sắp xếp riêng.
- **Queue (Cụ thể là Priority Queue / Min-Heap - Hàng đợi ưu tiên):** - *Dùng để:* Quản lý lịch trình và quyết định thứ tự xuất hiện của các từ vựng trong phiên học.
  - *Vì:* Mỗi từ vựng sẽ có một chỉ số thời gian cần ôn lại (`priority`). Hàng đợi ưu tiên Min-Heap giúp hệ thống luôn lấy ra được từ vựng có độ ưu tiên nhỏ nhất (tức là từ đang đến hạn hoặc quá hạn cần phải ôn tập ngay lập tức) với độ phức tạp cực kỳ tối ưu là $O(1)$.
- **Stack (Ngăn xếp):**
  - *Dùng để:* Lưu lại lịch sử các lượt trả lời (đúng/sai) của người dùng trong phiên học hiện tại.
  - *Vì:* Hoạt động theo cơ chế LIFO (Last In, First Out - Vào sau ra trước), Stack cực kỳ phù hợp để xây dựng tính năng Hoàn tác (Undo), cho phép lấy ra ngay lập tức trạng thái của từ vựng ở câu vừa trả lời để khôi phục lại nếu người dùng lỡ tay bấm nhầm.

## Compile và chạy
g++ -std=c++17 src/main.cpp src/functions.cpp -o app && ./app

## Chức năng
1. **Quản lý bộ từ điển:** Cho phép người dùng thêm từ vựng mới (gồm từ, nghĩa, ví dụ), chỉnh sửa nội dung nghĩa/ví dụ của từ hiện có, hoặc xóa hẳn một từ ra khỏi hệ thống.
2. **Học từ thông minh (Spaced Repetition):** Hệ thống tự động trích xuất từ vựng cần ôn nhất, người dùng xem từ, tự nhẩm nghĩa rồi lật mặt sau để đối chiếu và chọn "Đúng" hoặc "Sai". Hệ thống dựa vào đó để tự động tính toán lịch gặp lại từ đó.
3. **Tìm kiếm từ vựng:** Hỗ trợ tra cứu nhanh chóng nghĩa, ví dụ và các thông số thống kê (streak, số lần đúng/sai) của một từ bất kỳ trong cây từ điển.
4. **In danh sách từ vựng:** Liệt kê toàn bộ các từ vựng đang có trong hệ thống theo thứ tự Alphabet từ A-Z.
5. **Thống kê chuyên sâu:** Xem tỷ lệ trả lời chính xác tổng thể của toàn bộ bộ từ và hiển thị danh sách **Top 5 từ vựng hay quên nhất** (có số lần trả lời sai nhiều nhất) để người dùng tập trung chú ý.
6. **Hoàn tác (Undo):** Cho phép người dùng rút lại kết quả của câu vừa bấm (Đúng/Sai) để phục hồi lại các thông số cũ của từ vựng nếu lỡ tay thao tác sai.
7. **Tự động đồng bộ Tệp tin (File I/O):** Tự động tải (load) lại toàn bộ tiến độ học và dữ liệu từ file `flashcards.txt` khi mở app, đồng thời tự động lưu (save) lại mọi thay đổi vào file khi chọn thoát chương trình.

## Test cases
1. **Test Case 1: Kiểm tra thêm từ mới và tìm kiếm dữ liệu.**
   - *Thao tác:* Chọn chức năng 1 để thêm từ `"Apple"`, nghĩa `"Quả táo"`, ví dụ `"An apple a day"`. Sau đó chọn chức năng 5 để tìm kiếm từ `"Apple"`.
   - *Kết quả mong đợi:* Hệ thống thông báo thêm thành công. Khi tìm kiếm, màn hình hiển thị chính xác các thông tin nghĩa, ví dụ vừa nhập và các chỉ số thống kê ban đầu đều bằng 0.
2. **Test Case 2: Kiểm tra thuật toán Spaced Repetition khi trả lời ĐÚNG.**
   - *Thao tác:* Vào phiên học (Chức năng 4), hệ thống hiển thị từ `"Apple"`. Người dùng chọn kết quả `"Đúng"`.
   - *Kết quả mong đợi:* Chỉ số `streak` (chuỗi đúng) của từ tăng lên, `interval` (khoảng cách) tăng lên gấp đôi, từ này được cập nhật `priority` mới lớn hơn và được đẩy về phía sau trong hàng đợi ôn tập.
3. **Test Case 3: Kiểm tra thuật toán Spaced Repetition khi trả lời SAI.**
   - *Thao tác:* Vào phiên học (Chức năng 4), hệ thống hiển thị từ `"Apple"`. Người dùng chọn kết quả `"Sai"`.
   - *Kết quả mong đợi:* Chỉ số `streak` lập tức bị reset về `0`, `interval` co ngắn lại về mức tối thiểu, từ này nhận một `priority` thấp để đảm bảo nó quay trở lại vị trí đầu hàng đợi ưu tiên, buộc người dùng phải ôn lại ngay trong phiên học.
4. **Test Case 4: Kiểm tra tính năng Hoàn tác (Undo).**
   - *Thao tác:* Trong phiên học, người dùng bấm nhầm `"Sai"` cho từ `"Hello"`. Ngay sau đó bấm phím chọn chức năng `"Undo"` (Chức năng 8).
- *Kết quả mong đợi:* Hệ thống thông báo hoàn tác thành công. Số lần trả lời sai của từ `"Hello"` giảm đi 1, chỉ số `streak` và `priority` của từ quay trở lại chính xác trạng thái ngay trước khi bấm nhầm.
5. **Test Case 5: Kiểm tra tính năng Thống kê từ hay quên nhất.**
   - *Thao tác:* Tạo 3 từ `"One"`, `"Two"`, `"Three"`. Cố tình chọn trả lời `"Sai"` cho từ `"Two"` 5 lần, từ `"Three"` 2 lần trong phiên học. Sau đó mở chức năng Thống kê (Chức năng 7).
   - *Kết quả mong đợi:* Hệ thống tính toán chính xác tỷ lệ đúng tổng thể. Ở danh sách từ hay quên, từ `"Two"` phải xếp ở vị trí đầu tiên (do có 5 lần sai), theo sau là từ `"Three"` (2 lần sai).

## Cấu trúc file
src/
  main.cpp       — Menu và điều khiển luồng tương tác, nhận lựa chọn từ người dùng
  structures.h   — Khai báo các cấu trúc dữ liệu chính (FlashCard, Node, BST, Stack, FlashcardManager)
  functions.cpp  — Cài đặt chi tiết logic của các hàm quản lý, thuật toán Spaced Repetition và File I/O
