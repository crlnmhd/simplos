#ifndef UTILS_HPP_
#define UTILS_HPP_

class DeletedDefaultImplementations {
 private:
  DeletedDefaultImplementations() = delete;
  DeletedDefaultImplementations(const DeletedDefaultImplementations &) = delete;
  DeletedDefaultImplementations &operator=(
      const DeletedDefaultImplementations &) = delete;
  DeletedDefaultImplementations(DeletedDefaultImplementations &&) = delete;
  DeletedDefaultImplementations &operator=(DeletedDefaultImplementations &&) =
      delete;
};

#endif  // UTILS_HPP_
