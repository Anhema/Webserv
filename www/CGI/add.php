<?php
$raw_post_data = file_get_contents("php://input");

// Debug: Print raw POST data
echo "<br>Raw POST data:<br>";
echo $raw_post_data;

// Parse the POST data as needed
parse_str($raw_post_data, $_POST);

// Debug: Print parsed POST data
echo "Parsed POST data:<br>";
print_r($_POST);

// Debugging: Print environment variables
echo "<br><br>Environment variables:<br>";
print_r($_SERVER);

if (isset($_POST['num1']) && isset($_POST['num2']))
{
    // Retrieve the numbers from the form
    $num1 = $_POST['num1'];
    $num2 = $_POST['num2'];
    
    // Calculate the sum
    $sum = $num1 + $num2;
    
    // Display the result
    echo "$num1 + $num2 = $sum";
}
else
{
    echo "<br><br>Invalid input. Please enter two numbers.<br>";
}
?>
