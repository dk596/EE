$(document).ready(function(){
  // Get the selected node name from the dropdown on the main page
  var selectedNode = $("#node").val();
  // Extract the digits from the node name (e.g., 'node_1' -> '1')
  var nodeNumber = selectedNode.replace(/\D/g, ''); 
  var displayTitle = 'Sensor Node ' + (nodeNumber || selectedNode);
  
  $.ajax({
    url: "https://mediumvioletred-sandpiper-465466.hostingersite.com/Chartjs/data.php",
    method: "GET",
    success: function(data) {
      var x_axis = [];
      var y_axis = [];

      // Loop through data and keep ONLY records matching selectedNode
      for (var i = 0; i < data.length; i++) {
        if (data[i].node_name === selectedNode) {
          x_axis.push(data[i].time_received);
          y_axis.push(data[i].temperature);
        }
      }

      var chartdata = {
        labels: x_axis,
        datasets: [
          {
            label: displayTitle,
            backgroundColor: 'green)',
            borderColor: 'green',
            hoverBackgroundColor: 'green',
            hoverBorderColor: 'green',
            data: y_axis
          }
        ]
      };

      var ctx = $("#mycanvas");
    Chart.defaults.color = 'green';
    new Chart(ctx, {
      type: 'line',
      data: chartdata,
      options: {
        scales: {
          x: { title: { display: true, text: 'Time' } },
          y: { title: { display: true, text: 'Temperature' } }
        }
      }
    });
    },
    error: function(err) {
      console.error(err);
    }
  });
});
