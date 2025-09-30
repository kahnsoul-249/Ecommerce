#include <iostream>
#include <vector>
#include <string>
#include <algorithm>

using namespace std;

// Lớp giao diện trừu tượng cho các đối tượng có thể áp dụng giảm giá
class Discountable {
public:
    // Hàm ảo thuần túy để áp dụng giảm giá
    virtual double applyDiscount(double rate) = 0;
    // Hàm hủy ảo để hỗ trợ đa hình
    virtual ~Discountable() {}
};

// Lớp cơ sở cho sản phẩm
class Product : public Discountable {
private:
    int id;         // Mã định danh sản phẩm
    string name;    // Tên sản phẩm
    double price;   // Giá sản phẩm
    int stock;      // Số lượng tồn kho

public:
    // Constructor khởi tạo sản phẩm
    Product(int i, string n, double p, int s) : id(i), name(n), price(p), stock(s) {}
    // Destructor (rỗng vì không quản lý tài nguyên động)
    ~Product() {}

    // Getter cho các thuộc tính
    int getId() const { return id; }
    string getName() const { return name; }
    double getPrice() const { return price; }
    int getStock() const { return stock; }

    // Setter cho giá
    void setPrice(double p) { price = p; }

    // Cập nhật số lượng tồn kho (được khai báo là virtual)
    virtual void updateStock(int amount) {
        stock += amount;
        if (stock < 0) stock = 0;
    }

    // Áp dụng giảm giá (triển khai từ Discountable)
    double applyDiscount(double rate) override {
        double discount = price * rate;
        price -= discount;
        return discount;
    }

    // Nạp chồng toán tử == để so sánh sản phẩm theo ID
    bool operator==(const Product& other) const {
        return id == other.id;
    }

    // In thông tin sản phẩm
    virtual void print() const {
        cout << "Product ID: " << id << ", Name: " << name << ", Price: $" << price << ", Stock: " << stock << endl;
    }
};

// Lớp dẫn xuất kế thừa từ Product
class Electronics : public Product {
private:
    int warranty; // Thời hạn bảo hành (tháng)

public:
    // Constructor khởi tạo sản phẩm điện tử
    Electronics(int i, string n, double p, int s, int w) : Product(i, n, p, s), warranty(w) {}
    // Destructor (rỗng)
    ~Electronics() {}

    // Getter cho thời hạn bảo hành
    int getWarranty() const { return warranty; }

    // Ghi đè phương thức cập nhật tồn kho
    void updateStock(int amount) override {
        Product::updateStock(amount);
        if (amount < 0) {
            cout << "Electronics handling fee applied for ID " << getId() << "." << endl;
        }
    }

    // Ghi đè phương thức in thông tin
    void print() const override {
        Product::print();
        cout << "Warranty: " << warranty << " months" << endl;
    }
};

// Lớp template để quản lý danh sách tồn kho
template <typename T>
class InventoryList {
private:
    vector<T> data; // Danh sách các mục

public:
    // Thêm một mục vào danh sách
    void add(T item) {
        data.push_back(item);
    }

    // Xóa một mục khỏi danh sách
    void remove(T item) {
        auto it = remove_if(data.begin(), data.end(), [item](T ptr) { return *ptr == *item; });
        data.erase(it, data.end());
    }

    // Tìm mục theo ID
    T find(int id) const {
        for (const auto& item : data) {
            if (item->getId() == id) {
                return item;
            }
        }
        return nullptr;
    }

    // In tất cả các mục trong danh sách
    void print() const {
        for (const auto& item : data) {
            item->print();
        }
    }

    // Lấy danh sách dữ liệu
    const vector<T>& getData() const {
        return data;
    }

    // Lấy số lượng mục
    size_t size() const {
        return data.size();
    }
};

// Lớp giỏ hàng sử dụng InventoryList
class ShoppingCart {
private:
    InventoryList<Product*> items; // Danh sách sản phẩm trong giỏ
    double total;                 // Tổng giá trị giỏ hàng

public:
    // Constructor khởi tạo giỏ hàng
    ShoppingCart() : total(0.0) {}
    // Destructor (rỗng, vì con trỏ được giải phóng bên ngoài)
    ~ShoppingCart() {}

    // Thêm sản phẩm vào giỏ
    void addProduct(Product* p) {
        if (p->getStock() <= 0) {
            cout << "Error: Product ID " << p->getId() << " is out of stock!" << endl;
            return;
        }
        items.add(p);
        total += p->getPrice();
        p->updateStock(-1); // Giảm tồn kho
    }

    // Nạp chồng toán tử += để thêm sản phẩm
    ShoppingCart& operator+=(Product* p) {
        addProduct(p);
        return *this;
    }

    // Tính tổng giá trị
    double calculateTotal() const {
        return total;
    }

    // Áp dụng giảm giá cho toàn bộ giỏ
    void applyCartDiscount(double rate) {
        const auto& data = items.getData();
        for (const auto& item : data) {
            double disc = item->applyDiscount(rate);
            total -= disc;
        }
        cout << "Discount of " << rate * 100 << "% applied to cart." << endl;
    }

    // In nội dung giỏ hàng
    void printContents() const {
        cout << "Shopping Cart Contents:" << endl;
        items.print();
        cout << "Total: $" << total << endl;
    }
};

// Lớp đơn hàng
class Order {
private:
    int orderId;         // Mã đơn hàng
    ShoppingCart* cart;  // Giỏ hàng liên kết
    string date;         // Ngày đặt hàng

public:
    // Constructor khởi tạo đơn hàng
    Order(int oid, ShoppingCart* c, string d) : orderId(oid), cart(c), date(d) {}
    // Destructor (rỗng, không sở hữu giỏ hàng)
    ~Order() {}

    // In chi tiết đơn hàng
    void printDetails() const {
        cout << "Order ID: " << orderId << ", Date: " << date << endl;
        cart->printContents();
    }
};

// Hàm main để kiểm tra
int main() {
    // Tạo các đối tượng
    Product* p1 = new Product(1, "Book", 10.0, 5);
    Product* p2 = new Product(3, "Pen", 2.0, 0); // Hết hàng để kiểm tra lỗi
    Electronics* e1 = new Electronics(2, "Phone", 500.0, 3, 12);

    // Thể hiện kế thừa và ghi đè phương thức
    p1->updateStock(-1); // Cập nhật bình thường
    e1->updateStock(-1); // Ghi đè với thông báo phí xử lý

    // Kiểm tra nạp chồng toán tử ==
    if (*p1 == *p1) {
        cout << "Products with same ID are equal." << endl;
    } else {
        cout << "Products comparison failed." << endl;
    }

    // Tạo giỏ hàng và sử dụng InventoryList
    ShoppingCart cart;
    cart += p1; // Toán tử +=, thêm nếu có hàng
    cart += e1;
    cart += p2; // Lỗi: hết hàng
    cart.printContents();

    // Thể hiện giao diện: giảm giá đa hình
    cart.applyCartDiscount(0.1); // Giảm 10%
    cart.printContents();

    // Tạo đơn hàng và kiểm tra hoạt động
    Order order(1, &cart, "2025-09-30");
    order.printDetails();

    // Kiểm tra tính năng template
    InventoryList<Product*> inventory;
    inventory.add(p1);
    inventory.add(e1);
    Product* found = inventory.find(2);
    if (found) {
        cout << "Found item in inventory:" << endl;
        found->print();
    }
    inventory.remove(e1);
    cout << "Inventory after remove:" << endl;
    inventory.print();

    // Giải phóng bộ nhớ
    delete p1;
    delete p2;
    delete e1;

    return 0;
}