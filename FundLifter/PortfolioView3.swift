
import SwiftUI

var demoData = ["Phil Swanson", "Karen Gibbons", "Grant Kilman", "Wanda Green"]

struct PortfolioView3 : View {
  @State var selectKeeper = Set<String>()
  
  var body: some View {
    NavigationView {
      List(demoData, id: \.self, selection: $selectKeeper){ name in
        Text(name)
      }
      .navigationBarItems(trailing: EditButton())
      .navigationBarTitle(Text("Selection Demo \(selectKeeper.count)"))
    }
  }
  
  func setList() {
    
  }
}
