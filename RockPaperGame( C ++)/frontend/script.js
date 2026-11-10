
const API_URL = "http://localhost:8080";

// Get HTML elements
const choiceButtons = document.querySelectorAll(".choice-btn");

const userScoreElement = document.getElementById("user-score");
const computerScoreElement = document.getElementById("computer-score");
const drawScoreElement = document.getElementById("draw-score");

const userChoiceElement = document.getElementById("user-choice");
const computerChoiceElement = document.getElementById("computer-choice");
const resultElement = document.getElementById("result");
const statusElement = document.getElementById("status");
const resetButton = document.getElementById("reset-btn");

// Current scores
let userScore = 0;
let computerScore = 0;
let drawScore = 0;

// Handle a move
async function playGame(choice) {
    statusElement.textContent = "Playing...";

    choiceButtons.forEach(button => {
        button.disabled = true;
    });

    try {
        const response = await fetch(`${API_URL}/play`, {
            method: "POST",
            headers: {
                "Content-Type": "application/json"
            },
            body: JSON.stringify({ choice: choice })
        });

        if (!response.ok) {
            throw new Error("Server returned an error.");
        }

        const data = await response.json();

        // Display choices and result returned by C++
        userChoiceElement.textContent = "Your choice: " + data.user_choice;
        computerChoiceElement.textContent =
            "Computer choice: " + data.computer_choice;

        resultElement.textContent = data.result;

        // Update scores from backend response
        userScore = data.user_score;
        computerScore = data.computer_score;
        drawScore = data.draw_score;

        userScoreElement.textContent = userScore;
        computerScoreElement.textContent = computerScore;
        drawScoreElement.textContent = drawScore;

        statusElement.textContent = "Game ready!";

    } catch (error) {
        statusElement.textContent =
            "Cannot connect to the C++ server. Start the backend first.";
    } finally {
        choiceButtons.forEach(button => {
            button.disabled = false;
        });
    }
}

// Attach click events to the three buttons
choiceButtons.forEach(button => {
    button.addEventListener("click", () => {
        const choice = button.dataset.choice;
        playGame(choice);
    });
});

// Reset scores through the backend
resetButton.addEventListener("click", async () => {
    try {
        const response = await fetch(`${API_URL}/reset`, {
            method: "POST"
        });

        if (!response.ok) {
            throw new Error("Could not reset game.");
        }

        const data = await response.json();

        userScore = data.user_score;
        computerScore = data.computer_score;
        drawScore = data.draw_score;

        userScoreElement.textContent = userScore;
        computerScoreElement.textContent = computerScore;
        drawScoreElement.textContent = drawScore;

        userChoiceElement.textContent = "Your choice: -";
        computerChoiceElement.textContent = "Computer choice: -";
        resultElement.textContent = "Make your move!";

        statusElement.textContent = "Score reset successfully!";

    } catch (error) {
        statusElement.textContent =
            "Cannot connect to the C++ server. Start the backend first.";
    }
});
