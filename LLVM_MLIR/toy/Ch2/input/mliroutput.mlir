

module {
  func @multiply_transpose(%arg0: tensor<*xf64>, %arg1: tensor<*xf64>) -> tensor<*xf64> {
    %0 = "toy.transpose"(%arg0) : (tensor<*xf64>) -> tensor<*xf64>
    %1 = "toy.transpose"(%arg1) : (tensor<*xf64>) -> tensor<*xf64>
    %2 = "toy.mul"(%0, %1) : (tensor<*xf64>, tensor<*xf64>) -> tensor<*xf64>
    "toy.return"(%2) : (tensor<*xf64>) -> ()
  }
  func @main() {
    %0 = "toy.constant"() {value = dense<[[1.000000e+00, 2.000000e+00, 3.000000e+00], [4.000000e+00, 5.000000e+00, 6.000000e+00]]> : tensor<2x3xf64>} : () -> tensor<2x3xf64>
    %1 = "toy.constant"() {value = dense<[1.000000e+00, 2.000000e+00, 3.000000e+00, 4.000000e+00, 5.000000e+00, 6.000000e+00]> : tensor<6xf64>} : () -> tensor<6xf64>
    %2 = "toy.reshape"(%1) : (tensor<6xf64>) -> tensor<2x3xf64>
    %3 = "toy.generic_call"(%0, %2) {callee = @multiply_transpose} : (tensor<2x3xf64>, tensor<2x3xf64>) -> tensor<*xf64>
    %4 = "toy.generic_call"(%2, %0) {callee = @multiply_transpose} : (tensor<2x3xf64>, tensor<2x3xf64>) -> tensor<*xf64>
    %5 = "toy.generic_call"(%2, %3) {callee = @multiply_transpose} : (tensor<2x3xf64>, tensor<*xf64>) -> tensor<*xf64>
    %6 = "toy.transpose"(%0) : (tensor<2x3xf64>) -> tensor<*xf64>
    %7 = "toy.generic_call"(%6, %3) {callee = @multiply_transpose} : (tensor<*xf64>, tensor<*xf64>) -> tensor<*xf64>
    "toy.return"() : () -> ()
  }
}