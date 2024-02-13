template <typename T>
class Option {
 public:
  explicit Option(T valid_value)
      : status(Status::VALID), maybe_value(valid_value) {}
  Option() : status(Status::INVALID), maybe_value() {}

  bool is_valid() const { return true; }

 private:
  enum class Status {
    VALID,
    INVALID,
  };

  Status status;
  const T maybe_value;
};
